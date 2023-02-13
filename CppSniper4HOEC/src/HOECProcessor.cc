#include "HOECProcessor.h"
#include "CppSniper/CppSniper4HOEC.h"
#include <cstdint>
#include <iostream>
#include <unistd.h> 
#include <cassert>
#include "SniperKernel/SniperLog.h"
#include <chrono>
#include <deque>

HOECProcessor::HOECProcessor():
m_fragmentPool(20000),
m_processPointer(m_fragmentPool),
m_timeoutPointer(m_fragmentPool),
m_timeoutPatience(10),
m_processPatience(5)
{   
    m_mainThread = new std::thread(&HOECProcessor::mainThreadFunc, this);

    m_hoec = new CppSniper4HOEC("HOECConfig");
    m_workerThread = new std::thread(&HOECProcessor::workerThreadFunc, this);    
}

HOECProcessor::~HOECProcessor()
{
    LogInfo<<"*************************The input l1id*********"<<std::endl;
        for(auto num : m_l1idOrder){
            LogInfo<<num<<"   "<<std::endl;
        }

    m_mainThread->detach();
    m_workerThread->detach();
    //auto mainThreadID = m_mainThread->get_id();
    //auto workerThreadID = m_workerThread->get_id();
    //pthread_cancel(mainThreadID);
    //pthread_cancel(workerThreadID);

    delete m_hoec;
}

bool HOECProcessor:: put(const oec::EvsInTimeFragment& evtsPtr, oec::TimeoutInMs timeout){
    return m_queIn.PutElement(evtsPtr);
}

bool HOECProcessor::get(oec::EvsInTimeFragment& evtsPtr, oec::TimeoutInMs timeout){
    assert(evtsPtr.size() == 0);
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    auto spend_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    bool flag = false;
    while(spend_time.count() < timeout){
        if (m_queOut.TryGetElement(evtsPtr))
        {
            flag = true;
            break;
        }
        else {
            flag = false;
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            end = std::chrono::steady_clock::now();
            spend_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
        }
    }
    
    if(flag)    
        LogInfo<<"****************************DAQ get a fragment from HOEC************"<<evtsPtr.size()<<std::endl;
    return flag;
}

void HOECProcessor::oec_process(void* input, void* results)
{
    //std::cout<<"This is ES of Sniper"<<std::endl;

    //const std::vector<oec::OECRecEvt*>& evtVec = *(reinterpret_cast<std::vector<oec::OECRecEvt*>*>(input));
    //m_orderedQueue.enqueue(evtVec);
    //std::cout<<"#####################Successfully put evt in sortList########"<<std::endl;
    //sleep(1);
    //while(true){
    //    oec::OECRecEvt* event = m_orderedQueue.get_evt();
    //    if(event == nullptr) break;
    //    m_hoec->process(event, results);
    //}
}

void HOECProcessor::mainThreadFunc(){
    std::shared_ptr<oec::EvsInTimeFragment> evtsPtr;

    //根据第一个到来的时间片确定m_processorPointer的初始值
    evtsPtr.reset(new oec::EvsInTimeFragment(10));
    m_queIn.GetElement(*evtsPtr);
    LogInfo<<"***************Receive the fragment from DAQ***********"<<std::endl;
    oec::OECRecEvt* _firstEvt = ((oec::OECRecEvt*)(*evtsPtr->begin()));
    assert(_firstEvt->marker == 0x12345678);
    LogInfo<<"*************************The l1id is "<<_firstEvt->l1id<<"**************"<<std::endl;
    m_l1idOrder.push_back(_firstEvt->l1id);
    m_newestTime = _firstEvt->sec;
    m_fragmentPool.insertFrag(evtsPtr, _firstEvt->l1id, _firstEvt->sec, _firstEvt->nanoSec);
    m_processPointer.locate = _firstEvt->l1id % m_processPointer.arrayLen;//初始化m_processPointer
    m_timeoutPointer = m_processPointer;
    m_cleanTimer = _firstEvt->sec;
    processFragment();

    while(true){
        evtsPtr.reset(new oec::EvsInTimeFragment(15));//假设时间片内有 15个events,用于承接m_queIn中的数据
        if(!m_queIn.GetElement(*evtsPtr)){
            LogInfo<<"queue is waked up. The mainThread exit!"<<std::endl;
            break;
        }
        
        _firstEvt = ((oec::OECRecEvt*)(*evtsPtr->begin()));//此时间片内第一个事例的信息
        
        //判断TF是否在可接受的范围内
        if(!isAcceptable(_firstEvt)){
            LogInfo<<"The time of last cleaned: "<<(*m_timeoutPointer).timeSec<<" "<<(*m_timeoutPointer).nanoSec<<std::endl;
            LogInfo<<"The time of newly accepted evt: "<<_firstEvt->sec<<" "<<_firstEvt->nanoSec<<std::endl;
            LogError<<"Error: the received TF is not in the processing scope, Too old"<<std::endl;
            continue;
        }
        
        assert(_firstEvt->marker == 0x12345678);
        LogInfo<<"*******************Mian thread insert one TF, TFid is "<<_firstEvt->l1id<<"**************"<<std::endl;
        m_l1idOrder.push_back(_firstEvt->l1id);
        m_fragmentPool.insertFrag(evtsPtr, _firstEvt->l1id, _firstEvt->sec, _firstEvt->nanoSec);
        m_newestTime = std::max(m_newestTime, _firstEvt->sec);

        handleExceptions(_firstEvt->l1id);//新到的l1id 可能与异常有关

        processFragment();//尝试着把processPointer往下推；

        cleanTimeout();//检查超时 时间片

        return2DAQ();//取结果，返回给DAQ

    }

}

void HOECProcessor::handleExceptions(uint32_t l1id){
    return;
}

void HOECProcessor::processFragment(){

    while(true){
        HOECFragment& currentFrag = *(m_processPointer);
        if(currentFrag.stat == HOECFragment::Status::ready){
            currentFrag.stat = HOECFragment::Status::inWorker;
            m_iWorkerQ.PutElement(&currentFrag);
            m_processTimer = currentFrag.timeSec;

        }
        else if(currentFrag.stat == HOECFragment::Status::empty){//遇到一个空的,触发异常，或者等待下次
            if(m_newestTime - m_processTimer > m_processPatience ){//触发异常  
                currentFrag.timeSec = m_processTimer;//To do: 触发异常，具体行为待完善
                LogInfo<<"Exception: Out of patience, trigger an exception"<<std::endl;
                LogInfo<<"Exception TFid: "<<currentFrag.l1id<<" Locate: "<<m_processPointer.locate<<std::endl;
                currentFrag.stat = HOECFragment::Status::late;
            }
            else    break;//等待下次
        }
        else{
            LogError<<"Error processPointer is on an unexpected status  "<<currentFrag.stat<<std::endl;
            LogInfo<<"The l1id is "<<currentFrag.l1id<<std::endl;
            m_fragmentPool.snapShot();
            assert(false);
        }
        ++m_processPointer;
    }

    return;
}

void HOECProcessor::cleanTimeout(){
//清理ring的逻辑
//随着timeout Pointer的移动清理所有returned的frag，更新cleanTimer
//遇到了未处理的frag，打上保守tag， 更新cleanTimer
//遇到late frag 报一个warning

    while(true){//此循环是为了腾空FragmentRingArray
        HOECFragment& fragment = *(m_timeoutPointer);

        if(fragment.stat == HOECFragment::Status::returned){
            m_cleanTimer = fragment.timeSec;//更新移除ring的frag的时间
        }
        else {//如果没有被返回，则可能面临超时
            if(m_newestTime - m_cleanTimer < m_timeoutPatience)    break;//尚不需要被超时处理
            
            if(fragment.stat == HOECFragment::Status::ready){//时间片已经到达 但是必须超时返回
                //To do：将已经到达的时间片 打上保守的tag 返回
                m_cleanTimer = fragment.timeSec;
                m_queOut.PutElement(*(fragment.evtsPtr));
                fragment.stat = HOECFragment::Status::returned;
            }
            else if(fragment.stat == HOECFragment::Status::late){//时间片直到超时也没有到达
                LogFatal<<"Warning: clean one time out. The state is late. TFid: "<<fragment.l1id<<std::endl;
                fragment.stat = HOECFragment::Status::returned;
            }
            else{//To Do: 清理指针遇到了 inworker
                LogError<<"The timeoutPointer is on an unexpected status "<<fragment.stat<<std::endl;
                m_fragmentPool.snapShot();
                assert(false);
            }
        }

        LogInfo<<"one TF is cleaned from ring. TFid: "<<fragment.l1id<<" Locate: "<<m_timeoutPointer.locate<<std::endl;
        assert(fragment.stat == HOECFragment::Status::returned);
        fragment.evtsPtr = nullptr;
        fragment.stat = HOECFragment::Status::empty;
        ++m_timeoutPointer;
    }
    return;
}

void HOECProcessor::return2DAQ(){
    HOECFragment* retFrag;
    while(true){
        if(!(m_oWorkerQ.TryGetElement(retFrag)))    break;

        assert(retFrag->stat == HOECFragment::Status::inWorker || retFrag->stat == HOECFragment::Status::returned);
        m_queOut.PutElement(*(retFrag->evtsPtr));//结果返回给DAQ
        retFrag->evtsPtr = nullptr;
        
        if(retFrag->stat == HOECFragment::Status::inWorker)
            retFrag->stat = HOECFragment::Status::returned;
        else if(retFrag->stat == HOECFragment::Status::returned)
            retFrag->stat = HOECFragment::Status::empty;
    }
    return;
}

bool HOECProcessor::isAcceptable(oec::OECRecEvt* evt){
    int length = m_fragmentPool.getLength();
    int locate = (int)(evt->l1id% (uint32_t)length);
    int resistLength = length/4;//落后于timeoutPointer resisLength的TF就不被接受了
    //TODO: 时间判断和相对位置判断一起决定是否可接受
    if(m_timeoutPointer.locate - resistLength >= 0){
        if(locate <= m_timeoutPointer.locate && locate >= m_timeoutPointer.locate - resistLength)   return false;
    }
    else{
        int _bottom = m_fragmentPool.getLength() - (resistLength - m_timeoutPointer.locate);
        if(locate <= m_timeoutPointer.locate || locate >= _bottom)   return false;
    }
    return true;
}

void HOECProcessor::workerThreadFunc(){
    std::deque<std::pair<HOECFragment*, unsigned long>> fragments;
    
    while(true){
        HOECFragment* fragment = nullptr;
        if(!m_iWorkerQ.GetElement(fragment)){
            LogInfo<<"queue is waked up. One thread destroyed"<<std::endl;
            break;
        }
        
        assert(fragment != nullptr);
        fragments.push_back(std::make_pair(fragment, fragment->evtsPtr->size())); 
        
        //LogInfo<<"********************WOrker get a TF*****************TFid "<<fragment->l1id<<std::endl;
        for(auto evt : *(fragment->evtsPtr)){
            oec::OECRecEvt* event = (oec::OECRecEvt*)evt;
            //LogInfo<<"************Event time(put to hec task) "<<event->sec<<" "<<event->nanoSec<<std::endl;
        }

        for(auto evt : *(fragment->evtsPtr)){
            if(m_hoec->process((oec::OECRecEvt*)evt) == nullptr){
                continue;
            }
            
            fragments.front().second--;
            if(fragments.front().second == 0){
                showTagResult(nullptr);//debug用， 打印HOEC处理结果
                m_oWorkerQ.PutElement(fragments.front().first);
                fragments.pop_front();
            }
        }  
    }    
}

void HOECProcessor::showTagResult(HOECFragment* fragment){
    //LogInfo<<"The l1id of processed fragment"
    return;
}

extern "C" {
    oec::AlgInterface *create_processor_async()
    {
        return new HOECProcessor;
    }

    bool destroy_resource(oec::AlgInterfaceAsync* alg_object)
    {   

        delete alg_object;
        alg_object = nullptr;
        return true;
    }
}
