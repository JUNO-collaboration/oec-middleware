#ifndef LOEC_PROCESSOR_H
#define LOEC_PROCESSOR_H

#include "oec_com/AlgInterface.h"
#include "CppSniper/CppSniper4LOEC.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <cstdint>

class LOECProcessor: public oec::AlgInterface
{
public:
    LOECProcessor(int thrNum);
    virtual ~LOECProcessor();

    virtual void oec_process(void*, void*);

private:
    std::vector<std::thread*> m_threads;
    std::vector<CppSniper4LOEC*> m_cppSnps;

    int initialNum; 
    std::mutex initialMutex;
    std::condition_variable checkInitialize;
    

    std::deque<void*> jobQueue;
    std::mutex jobQueueMutex;
    std::condition_variable workToBeDone;
    

    int jobDoneNum;
    std::mutex doneMutex; 
    std::condition_variable doneJob;
    

    void thrdWork(int);
    void* getJob();
    void finishJob();
    void initFinalize(int thrNum);
};

#endif