#ifndef HOEC_PROCESSOR_H
#define HOEC_PROCESSOR_H

#include "oec_com/AlgInterface.h"
#include <cstdint>
#include <list>
#include <mutex>
#include <condition_variable>
#include "oec_com/QueueForOEC.h"
#include "FragmentRingArray.h"
#include <chrono>

class CppSniper4HOEC;
class RecEvt;

class HOECProcessor : public oec::AlgInterfaceAsync
{
public:
    HOECProcessor();
    virtual ~HOECProcessor();
    
    bool virtual put(const oec::EvsInTimeFragment&, oec::TimeoutInMs = 1000) override;
    bool virtual get(oec::EvsInTimeFragment&, oec::TimeoutInMs = 1000) override; 
    
    std::vector<uint32_t> m_l1idOrder;//对功能无用，debug专用
    
private://成员变量的初始化顺序，依照此顺序

    QueueForOEC<oec::EvsInTimeFragment> m_queIn;//用于和daq异步交互的队列
    QueueForOEC<oec::EvsInTimeFragment> m_queOut;

    //控制线程
    FragmentRingArray m_fragmentPool;
    FragmentRingArray::Iterator m_processPointer;
    FragmentRingArray::Iterator m_timeoutPointer;
    uint32_t m_currentTime;    //HOEC管理的时间片中 最新的时间
    uint32_t m_timeoutPatience;
    std::thread* m_mainThread;//控制线程，控制HOEC的逻辑流程
    void mainThreadFunc();
    void handleExceptions(uint32_t l1id);
    void processFragment();
    void cleanTimeout();
    void return2DAQ();
    virtual void oec_process(void* event, void* results);


    //worker线程
    QueueForOEC<HOECFragment*> m_iWorkerQ;//控制线程传递数据给worker
    QueueForOEC<HOECFragment*> m_oWorkerQ;//控制线程从worker取数据
    CppSniper4HOEC* m_hoec;   //for high level OEC in SNiPER
    std::thread* m_workerThread;
    void workerThreadFunc();
};


#endif
