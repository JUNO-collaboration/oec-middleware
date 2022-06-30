#ifndef LOEC_PROCESSOR_H
#define LOEC_PROCESSOR_H

#include "oec_com/AlgInterface.h"
#include "CppSniper/CppSniper4LOEC.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <cstdint>
#include "oec_com/QueueForOEC.h"

class LOECProcessor: public oec::AlgInterfaceAsync
{
public:
    LOECProcessor(int thrNum);
    virtual ~LOECProcessor();

    bool virtual put(const oec::EvsInTimeFragment&, oec::TimeoutInMs = 1000) override;
    bool virtual get(oec::EvsInTimeFragment&, oec::TimeoutInMs = 1000) override;

private:
    std::thread* m_mainThread;
    virtual void oec_process(void*, void*);
    void mainThreadFunc();
    QueueForOEC<oec::EvsInTimeFragment> m_queIn;//用于和daq异步交互的队列
    QueueForOEC<oec::EvsInTimeFragment> m_queOut;

    std::vector<std::thread*> m_threads;
    std::vector<CppSniper4LOEC*> m_cppSnps;

    int initialNum; 
    std::mutex initialMutex;
    std::condition_variable checkInitialize;
    
    QueueForOEC<void*> m_jobQueue;//用来给工作线程竞争任务用的
    
    int jobDoneNum;
    std::mutex doneMutex; 
    std::condition_variable doneJob;
    
    void thrdWork(int);
    void finishJob();
    void initFinalize(int thrNum);
};

#endif