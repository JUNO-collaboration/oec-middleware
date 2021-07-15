#include "LOECProcessor.h"
#include "TROOT.h"
#include <iostream>

LOECProcessor::LOECProcessor(int thrNum = 1){
    std::cout<<"Start multi-thread version"<<std::endl;
    
    initialNum = 0;

    if (thrNum > 1) ROOT::EnableThreadSafety();

    //Py_Initialize();
    //PyEval_InitThreads();
    //PyEval_ReleaseLock();
    
    m_threads.reserve(thrNum);
    m_cppSnps.reserve(thrNum);

    for(int i = 0;i < thrNum; i++){
        std::cout<<"Try to create a Thread"<<std::endl;
        //CppSniper4LOEC m_rec("LOECWaveformRec");
        m_cppSnps.push_back(new CppSniper4LOEC("LOECWaveformRec"));
        m_threads.push_back(new boost::thread(boost::bind(&LOECProcessor::thrdWork,this,i)));
    }
    //initFinalize(thrNum);
   
    std::cout<<"********************* All threads has been created *******************"<<std::endl;
}

LOECProcessor::~LOECProcessor(){
    for(int i = 0; i < m_threads.size(); i++){
        workToBeDone.notify_all();
        m_threads[i]->interrupt();
        delete m_threads[i];
        delete m_cppSnps[i];
    }
}

void LOECProcessor::oec_process(void* input, void* /*nullptr*/){
    jobDoneNum = 0;
    std::vector<oec::EventDepository*>* evDepoes = reinterpret_cast<std::vector<oec::EventDepository*>*>(input);
    jobQueue = *evDepoes;
    workToBeDone.notify_all();

    
    while(true){
        //std::cout<<"Check if all jobs have been completed"<<std::endl;
        boost::mutex::scoped_lock lock(doneMutex);
        doneJob.wait(lock);
        if(jobDoneNum == evDepoes->size()) break;
    }
    
    std::cout<<"****************All jobs has been finished******************"<<std::endl;
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
        std::cout<<"Thread "<<boost::this_thread::get_id()<< "is ready for a job"<<std::endl;
        m_rec.process(getJob());
        //getJob();
        finishJob();
        //std::cout<<"Thread "<<boost::this_thread::get_id()<< "finished a job"<<std::endl;
    }    
}

oec::EventDepository* LOECProcessor::getJob(){
    //std::cout<<"Try to get a job"<<std::endl;
    boost::mutex::scoped_lock lock(jobQueueMutex);
    while(true){
        boost::this_thread::interruption_point();
        if(jobQueue.empty()) workToBeDone.wait(lock);
        else break;
    }
    oec::EventDepository* job = jobQueue.back();
    jobQueue.pop_back();
    //std::cout<<"Got one job"<<std::endl;
    return job;
}

void LOECProcessor::finishJob(){
    boost::mutex::scoped_lock lock(doneMutex);
    jobDoneNum++;
    doneJob.notify_one();
}

void LOECProcessor::initFinalize(int thrNum){
    while(true){
        boost::mutex::scoped_lock lock(initialMutex);
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
