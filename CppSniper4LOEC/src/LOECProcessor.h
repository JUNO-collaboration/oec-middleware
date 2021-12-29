#ifndef LOEC_PROCESSOR_H
#define LOEC_PROCESSOR_H

#include "oec_com/AlgInterface.h"
#include "CppSniper/CppSniper4LOEC.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <deque>
#include <cstdint>


class EventDepository;
class CppSniper4LOEC;

class LOECProcessor: public oec::AlgInterface
{
public:
    LOECProcessor(int thrNum);
    virtual ~LOECProcessor();

    virtual void oec_process(void*, void*);

private:
    std::vector<boost::thread*> m_threads;
    std::vector<CppSniper4LOEC*> m_cppSnps;

    int initialNum; 
    boost::mutex initialMutex;
    boost::condition checkInitialize;
    

    std::deque<void*> jobQueue;
    boost::mutex jobQueueMutex;
    boost::condition workToBeDone;
    

    int jobDoneNum;
    boost::mutex doneMutex; 
    boost::condition doneJob;
    

    void thrdWork(int);
    void* getJob();
    void finishJob();
    void initFinalize(int thrNum);
};

#endif