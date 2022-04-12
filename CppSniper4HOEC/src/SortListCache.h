#ifndef SORT_LIST_CACHE
#define SORT_LIST_CACHE

#include <deque>
#include <mutex>
#include <condition_variable>
#include "oec_com/OEC_define.h"
#include <thread>
#include <vector>
#include <queue>

class OECRecEvtCmp{
public:
    bool operator()(oec::OECRecEvt* a, oec::OECRecEvt* b);
};

class SortListCache{
public:
    SortListCache(uint64_t cacheTime = 1, uint32_t cacheNumb = 5);//指定最大缓存多长时间，最大缓存数量
    ~SortListCache();
    
    void enqueue(const std::vector<oec::OECRecEvt*>& input);
    oec::OECRecEvt* get_evt();//注意，如果没有事例可以获得，返回一个nullptr
    
private:
    uint64_t m_cacheTime;//单位是秒
    uint64_t m_maximTime;//单位是秒
    uint32_t m_cacheNumb;

    std::deque<oec::OECRecEvt*>                       m_recvList;
    std::priority_queue<oec::OECRecEvt*, std::deque<oec::OECRecEvt*>, OECRecEvtCmp> m_sortList;
    std::deque<oec::OECRecEvt*>                       m_dispList;
    
    

    std::mutex m_rMutex;
    std::mutex m_dMutex;
    
    std::condition_variable m_sortCondi;
    std::condition_variable m_dispCondi;

    std::thread m_sortThrd;
    
    void thread_work();
    void en_sortList();//将m_recvList中的事例转移到m_sortList中排序
    void en_dispList();
    
    bool check_enDsp();
};
 

#endif 