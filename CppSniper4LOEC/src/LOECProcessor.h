#ifndef LOEC_PROCESSOR_H
#define LOEC_PROCESSOR_H

#include "OEC_com/oec_com/EventDepository.h"
#include "OEC_com/oec_com/AlgInterface.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
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
    

    std::vector<oec::EventDepository*> jobQueue;
    boost::mutex jobQueueMutex;
    boost::condition workToBeDone;
    oec::EventDepository* getJob();

    int jobDoneNum;
    boost::mutex doneMutex; 
    boost::condition doneJob;
    void finishJob();

    void thrdWork();
    
    
};

#endif