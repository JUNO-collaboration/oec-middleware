#include "LOECProcessor.h"
#include "TROOT.h"
#include <iostream>
#include "SniperKernel/SniperLog.h"

LOECProcessor::LOECProcessor(int thrNum = 1){
    std::cout<<"Start multi-thread version"<<std::endl;
    LogInfo<<__LINE__<<this<<std::endl;
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
    //initFinalize(thrNum);
   
    std::cout<<"********************* All threads has been created *******************"<<std::endl;
}

LOECProcessor::~LOECProcessor(){
    for(int i = 0; i < m_threads.size(); i++){
        workToBeDone.notify_all();
        //m_threads[i]->interrupt();
        delete m_threads[i];
        delete m_cppSnps[i];
    }
}

void LOECProcessor::oec_process(void* vec_data, void* /*nullptr*/){
    std::cout<<"this address "<<this<<std::endl;
    jobDoneNum = 0;
   
    std::vector<void*>& events = *reinterpret_cast<std::vector<void*>*>(vec_data);
    LogInfo<<__LINE__<<std::endl;
    if(events.size() == 0){
        return;
    }

    
    for(void* evt : events){
        jobQueue.push_back(evt);
    }
    
    

    workToBeDone.notify_all();

    {
        std::unique_lock<std::mutex> lock(doneMutex);
        doneJob.wait(lock, [this, events]{return jobDoneNum == events.size();});
    }
    
    
    LogInfo<<"****************All jobs has been finished******************"<<std::endl;
    //std::cout<<"****************        ps is done        ******************"<<std::endl;

    return;
}

void LOECProcessor::thrdWork(int i){
    CppSniper4LOEC& m_rec = *m_cppSnps[i];
    m_rec.initialize();
    std::cout<<"Initialize a CppSnipper"<<std::endl;
    
    initialMutex.lock();
    initialNum++;
    initialMutex.unlock();
    //checkInitialize.notify_one();
    
    while(true){
        //std::cout<<"Thread "<<boost::this_thread::get_id()<< "is ready for a job"<<std::endl;
        LogInfo<<__LINE__<<std::endl;
        m_rec.process(getJob());
        //getJob();
        finishJob();
        //std::cout<<"Thread "<<boost::this_thread::get_id()<< "finished a job"<<std::endl;
    }    
}

void* LOECProcessor::getJob(){
    //std::cout<<"Try to get a job"<<std::endl;
    void* job(nullptr);
    
    {
        std::unique_lock<std::mutex> lock(jobQueueMutex);
        workToBeDone.wait(lock, [this]{return !(this->jobQueue.empty());});
        job = jobQueue.front();
        jobQueue.pop_front();   
    }

    return job;
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
    oec::AlgInterface *create_processor()
    {   
        
        return new LOECProcessor(1);
    }
}
