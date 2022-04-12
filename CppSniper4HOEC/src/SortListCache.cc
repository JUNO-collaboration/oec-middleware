#include "SortListCache.h"
#include <iostream>

bool OECRecEvtCmp::operator()(oec::OECRecEvt* a, oec::OECRecEvt* b){//比较a，b两个事例的时间顺序，a先于b返回true
    
    return a->sec > b->sec || (a->sec == b->sec && a->nanoSec > b-> nanoSec);
}


SortListCache::SortListCache(uint64_t cacheTime, uint32_t cacheNumb):
m_cacheTime(cacheTime),
m_cacheNumb(cacheNumb),
m_recvList(),
m_sortList(),
m_dispList(),
m_sortThrd(&SortListCache::thread_work, this){
    std::cout<<"########################initializing a sort list cache"<<std::endl;
    return;
}


oec::OECRecEvt* SortListCache::get_evt(){
    oec::OECRecEvt* evt = nullptr;

    {
        std::unique_lock<std::mutex> lock(m_dMutex);
        if(m_dispList.empty())
            return evt;
        evt = m_dispList.front();
        m_dispList.pop_front();
    }

    return evt;
}


void SortListCache::enqueue(const std::vector<oec::OECRecEvt*>& input){
    if(input.empty()) return;
    {
        std::unique_lock<std::mutex> lock(m_rMutex);
        for(auto evt : input){
            m_recvList.push_back(evt);
        }
    }
    m_sortCondi.notify_one();
    return;
}


void SortListCache::thread_work(){
    while(true){
        en_sortList();
        en_dispList();
    }
}

void SortListCache::en_sortList(){
    oec::OECRecEvt* _evt = nullptr;
    
    {//从缓存队列中取出一个元素
        std::unique_lock<std::mutex> lock(m_rMutex);
        m_sortCondi.wait(lock, [this]{return !this->m_recvList.empty();});
        _evt = m_recvList.front();
        m_recvList.pop_front();
    }
    
    if(_evt->sec > m_maximTime)
        m_maximTime = _evt->sec;
    m_sortList.push(_evt);

    return;
}

void SortListCache::en_dispList(){
    while(m_sortList.size() > m_cacheNumb){
        oec::OECRecEvt* _evt = m_sortList.top();
        
        if(m_maximTime - _evt->sec < m_cacheTime)
            break;
        
        m_sortList.pop();
        {
            std::unique_lock<std::mutex> lock(m_dMutex);
            m_dispList.push_back(_evt);
        }
    }
}



