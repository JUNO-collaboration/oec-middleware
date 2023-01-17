#include "LOECProcessor.h"
#include "TROOT.h"
#include <iostream>
#include "SniperKernel/Sniper.h"
#include "SniperKernel/SniperLog.h"
#include <juno_pack/Event.h>//fixme：debug用，本不必依赖
#include "oec_com/OEC_define.h"//fixme：debug用，本不必依赖

LOECProcessor::LOECProcessor(int thrNum = 1){
    std::cout<<"Start multi-thread version"<<std::endl;
    LogInfo<<__LINE__<<std::endl;

    ROOT::EnableThreadSafety();

    //Py_Initialize();
    //PyEval_InitThreads();
    //PyEval_ReleaseLock();

    //读取配置
    auto fcfg = std::string{getenv("OFFLINE_DIR")} + "/config/MiddlewareConfig.json";
    auto jtask = Sniper::eval(fcfg.c_str());
    m_configTask = dynamic_cast<Task*>(jtask);
    m_configSvc = dynamic_cast<MiddlewareConfigSvc*>(m_configTask->find("MiddlewareConfigSvc"));
    thrNum = m_configSvc->getThrdNum();
    
    m_threads.reserve(thrNum);
    m_cppSnps.reserve(thrNum);
    for(int i = 0;i < thrNum; i++){
        std::cout<<"*****************************Try to create a worker task "<<i<<"/"<<thrNum<<std::endl;
        m_cppSnps.push_back(new CppSniper4LOEC());
        std::cout<<"******************************Try to initialize worker task"<<i<<"/"<<thrNum<<std::endl;
        m_cppSnps[i]->initialize();
        //m_threads.push_back(new std::thread(&LOECProcessor::thrdWork,this,i));
    }
    for(int i = 0; i < thrNum; i++){
        std::cout<<"*****************************Try to create a Thread "<<i<<"/"<<thrNum<<std::endl;
        m_threads.push_back(new std::thread(&LOECProcessor::thrdWork,this,i));
    }

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
    if(events.size() == 0){
        LogInfo<<"Error, there is no events in the fragment"<<std::endl;
        return;
    }

    for(void* evt : events){
        m_jobQueue.PutElement(evt);
    }


    {//确认时间片内的所有的事例都处理完毕
        std::unique_lock<std::mutex> lock(doneMutex);
        doneJob.wait(lock, [this, events]{return jobDoneNum == (int)events.size();});
        LogInfo<<"****************All events in fragment is processed"<<std::endl;
    }

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
}

void LOECProcessor::thrdWork(int i){
    CppSniper4LOEC& m_rec = *m_cppSnps[i];
    //m_rec.initialize();
    //LogInfo<<"Initialize a CppSnipper"<<std::endl;
    //
    //initialMutex.lock();
    //initialMutex.unlock();
    //checkInitialize.notify_one();
    
    while(true){
        LogInfo<<__LINE__<<std::endl;
        void* event;
        if(!m_jobQueue.GetElement(event)){
            LogInfo<<"queue is waked up. One thread destroyed"<<std::endl;
            break;
        }
        LogInfo<<__LINE__<<std::endl;
        m_rec.process(event);

        finishJob();
    }    
}

void LOECProcessor::finishJob(){
    std::unique_lock<std::mutex> lock(doneMutex);
    jobDoneNum++;
    doneJob.notify_one();
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
