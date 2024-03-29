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
#include "oec_com/OEC_define.h"

//**************异常处理思路***************
//正常情况：为了保证数据流能够正常通过HOEC，每次移动pointer，和向环形buffer塞入的过程中都需要预先检查格子的状态
//如果格子状态不符合预期不符合预期，就称之为异常

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
    uint32_t m_processTimer;//上一次处理事例的时间
    uint32_t m_processPatience;
    FragmentRingArray::Iterator m_timeoutPointer;
    uint32_t m_newestTime;    //HOEC管理的时间片中 最新的时间
    uint32_t m_cleanTimer;//上一次清理的时间
    uint32_t m_timeoutPatience;//HOEC中能够容忍的最大停留时间
    std::thread* m_mainThread;//控制线程，控制HOEC的逻辑流程
    void mainThreadFunc();
    void handleExceptions(uint32_t l1id);
    void processFragment();
    void cleanTimeout();
    void return2DAQ();
    bool isAcceptable(oec::OECRecEvt* evt);
    virtual void oec_process(void* event, void* results);

    //worker线程
    QueueForOEC<HOECFragment*> m_iWorkerQ;//控制线程传递数据给worker
    QueueForOEC<HOECFragment*> m_oWorkerQ;//控制线程从worker取数据
    CppSniper4HOEC* m_hoec;   //for high level OEC in SNiPER
    std::thread* m_workerThread;
    void workerThreadFunc();
    void showTagResult(HOECFragment* fragment);

    //用于debug的记录
};


#endif
