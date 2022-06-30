#include "LOECProcessor.h"
#include "TROOT.h"
#include <iostream>
#include "SniperKernel/SniperLog.h"
#include <juno_pack/Event.h>//fixme：debug用，本不必依赖
#include "oec_com/OEC_define.h"//fixme：debug用，本不必依赖

LOECProcessor::LOECProcessor(int thrNum = 1){
    std::cout<<"Start multi-thread version"<<std::endl;
    LogInfo<<__LINE__<<std::endl;
    initialNum = 0;

    ROOT::EnableThreadSafety();

    //Py_Initialize();
    //PyEval_InitThreads();
    //PyEval_ReleaseLock();
    
    m_threads.reserve(thrNum);
    m_cppSnps.reserve(thrNum);
    for(int i = 0;i < thrNum; i++){
        LogInfo<<"Try to create a Thread"<<std::endl;
        
        m_cppSnps.push_back(new CppSniper4LOEC());
        m_threads.push_back(new std::thread(&LOECProcessor::thrdWork,this,i));
    }
    
    initFinalize(thrNum);//用于同步：确认所有的工作线程都已初始化完毕

    m_mainThread = new std::thread(&LOECProcessor::mainThreadFunc, this);

    std::cout<<"********************* All threads has been created *******************"<<std::endl;
}

LOECProcessor::~LOECProcessor(){
    m_mainThread->detach();
    for(int i = 0; i < m_threads.size(); i++){
        m_threads[i]->detach();
        delete m_threads[i];
        delete m_cppSnps[i];
    }
}

bool LOECProcessor::put(const oec::EvsInTimeFragment& evs, oec::TimeoutInMs timeout){
    return m_queIn.PutElement(evs);
}

bool LOECProcessor::get(oec::EvsInTimeFragment& evs, oec::TimeoutInMs timeout){
    //debug:强行让等待时间变为5s
    timeout = 10000;


    assert(evs.size() == 0);
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    auto spend_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    bool flag = false;
    while(spend_time.count() < timeout){
        if (m_queOut.TryGetElement(evs))
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
    LogInfo<<"****************************DAQ get a fragment from LOC************"<<std::endl;
    return flag;
}

void LOECProcessor::oec_process(void* fragment, void* /*nullptr*/){

    LogInfo<<"******************** processing one fragment*************"<<std::endl;
    {
        std::unique_lock<std::mutex> lock(doneMutex);
        jobDoneNum = 0;
    }
    oec::EvsInTimeFragment events = *((oec::EvsInTimeFragment*)fragment);
    LogInfo<<__LINE__<<std::endl;
    if(events.size() == 0){
        LogInfo<<"Error, there is no events in the fragment"<<std::endl;
        return;
    }

    for(void* evt : events){
        m_jobQueue.PutElement(evt);
    }

    LogInfo<<__LINE__<<std::endl;
    {//确认时间片内的所有的事例都处理完毕
        std::unique_lock<std::mutex> lock(doneMutex);
        doneJob.wait(lock, [this, events]{return jobDoneNum == (int)events.size();});
        LogInfo<<"****************All events in fragment is processed"<<std::endl;
    }
    LogInfo<<__LINE__<<std::endl;
    {//debug
        LogInfo<<"*****************The fragment size is "<<events.size()<<std::endl;
        junoread::Event _event(reinterpret_cast<uint8_t*>(events[events.size() - 1]));
        std::pair<uint8_t*, uint8_t*> recResultInterval = _event.indexOf(junoread::Event::EventIndex::REC_RESULT);
        oec::OECRecEvt* recResult = reinterpret_cast<oec::OECRecEvt*>(*reinterpret_cast<uint64_t*>(recResultInterval.first + 4));
        assert(recResult->marker == 0x12345678);
    }

    return;
}

void LOECProcessor::mainThreadFunc(){
    LogInfo<<"****************The main thread Starts *****************"<<std::endl;
    while(true){
        oec::EvsInTimeFragment evs;
        if(!m_queIn.GetElement(evs)){
            LogInfo<<"queue is waked up."<<std::endl;
            m_jobQueue.WakeupQueue();
            break;
        }
        LogInfo<<"****************** Got a fragment. events num in the fragment "<<evs.size()<<std::endl;
        oec_process(&evs, nullptr);

        LogInfo<<"***********************push events in Output que"<<std::endl;
        if(!m_queOut.PutElement(evs)){ // 如果塞值失败，应该退出线程
            LogInfo<<"**************output queue is waked up***************"<<std::endl;
            break;
        }
    }

    oec_process(nullptr, nullptr);
}

void LOECProcessor::thrdWork(int i){
    CppSniper4LOEC& m_rec = *m_cppSnps[i];
    m_rec.initialize();
    LogInfo<<"Initialize a CppSnipper"<<std::endl;
    
    initialMutex.lock();
    initialNum++;
    initialMutex.unlock();
    checkInitialize.notify_one();
    
    while(true){
        LogInfo<<__LINE__<<std::endl;
        void* event;
        if(!m_jobQueue.GetElement(event)){
            LogInfo<<"queue is waked up. One thread destroyed"<<std::endl;
            break;
        }
        m_rec.process(event);

        finishJob();
    }    
}

void LOECProcessor::finishJob(){
    std::unique_lock<std::mutex> lock(doneMutex);
    jobDoneNum++;
    doneJob.notify_one();
}

void LOECProcessor::initFinalize(int thrNum){    //FIX ME
    while(true){
        LogInfo<<__LINE__<<std::endl;
        std::unique_lock<std::mutex> lock(initialMutex);
        if(initialNum == thrNum) break;
        checkInitialize.wait(lock);
    }
}

extern "C" {
    oec::AlgInterface *create_processor_async()
    {       
        return new LOECProcessor(1);
    }

    bool destroy_resource(oec::AlgInterfaceAsync* alg_object)
    {   
        delete alg_object;
        alg_object = nullptr;
        return true;
    }
}
