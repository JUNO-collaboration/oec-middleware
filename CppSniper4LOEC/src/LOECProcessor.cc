#include "LOECProcessor.h"
#include "CppSniper/CppSniper4LOEC.h"
#include <iostream>

LOECProcessor::LOECProcessor(int thrNum = 1){
    std::cout<<"Start multi-thread version"<<std::endl;

    m_threads.reserve(thrNum);
    for(int i = 0;i < thrNum; i++){
        std::cout<<"Try to create a Thread"<<std::endl;
        m_threads.push_back(new boost::thread(boost::bind(&LOECProcessor::thrdWork,this)));
    }
    
}

LOECProcessor::~LOECProcessor(){
    for(int i = 0; i < m_threads.size(); i++){
        delete m_threads[i];
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
    std::cout<<"****************        ps is done        ******************"<<std::endl;
    return;
}

void LOECProcessor::thrdWork(){
    CppSniper4LOEC m_rec("LOECWaveformRec");
    std::cout<<"Initialize a CppSnipper"<<std::endl;
    
    while(true){
        std::cout<<"Thread "<<boost::this_thread::get_id()<< "is ready for a job"<<std::endl;
        m_rec.process(getJob());
        finishJob();
    }    
}

oec::EventDepository* LOECProcessor::getJob(){
    //std::cout<<"Try to get a job"<<std::endl;
    boost::mutex::scoped_lock lock(jobQueueMutex);
    while(true){
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


extern "C" {
    oec::AlgInterface *create_processor()
    {
        return new LOECProcessor(2);
    }
}
