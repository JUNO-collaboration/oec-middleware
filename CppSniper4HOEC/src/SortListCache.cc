#include "SortListCache.h"


SortListCache::SortListCache(uint64_t cacheTime, float dispRate):
m_cacheTime(cacheTime),
m_dispRate(dispRate),
m_recvList(),
m_sortList(),
m_dispList(),
m_splicePoint(m_sortList.begin()),
m_spliceCount(0),
m_sortListLen(0),
m_sortThrd(&SortListCache::thread_work, this){
    return;
}


void SortListCache::push_back(junoread::Event* const & value){
    m_rMutex.lock();
    m_recvList.push_back(value);
    m_rMutex.unlock();

    m_sortCondi.notify_one();

    return;
}


void SortListCache::enqueue(std::list<junoread::Event*>& input){
    if(input.empty()) return;

    m_rMutex.lock();
    m_recvList.splice(m_recvList.end(), input);
    m_rMutex.unlock();

    m_sortCondi.notify_one();
    return; 
}


void SortListCache::disptch(std::list<junoread::Event*>& output){
    std::unique_lock<std::mutex> _lock(m_dMutex);
    while(true){
        if(m_dispList.empty()){
            m_dispCondi.wait(_lock);
        }
        else{
            break;
        }
    }

    output.splice(output.end(), m_dispList);
    
    return;
}

void SortListCache::thread_work(){
    while(true){
        en_sortList();
        en_dispList();
    }
}

void SortListCache::en_sortList(){
    std::list<junoread::Event*> _input;
    get_re_List(_input);
    insert_sort(_input);
    return;
}

void SortListCache::en_dispList(){
    if(!check_enDsp())
        return;

    m_dMutex.lock();
    m_dispList.splice(m_dispList.end(), m_sortList, m_sortList.begin(), m_splicePoint);
    m_sortListLen = m_sortListLen - m_spliceCount + 1;//切割点依然留在m_sortList
    m_splicePoint = (m_sortList.end()--);
    m_spliceCount = m_sortListLen;
    m_dMutex.unlock();
    m_dispCondi.notify_one();
    return;
}


void SortListCache::get_re_List(std::list<junoread::Event*>& list){
    std::unique_lock<std::mutex> _rlock(m_rMutex);
    while(true){
        if(m_recvList.empty()){
            m_sortCondi.wait(_rlock);
        }
        else{
            break;
        }
    }
    list.splice(list.end(), m_recvList);
    _rlock.unlock();

    list.sort(&SortListCache::evt_tm_comp);
    return;
}

void SortListCache::insert_sort(std::list<junoread::Event*>& list){
    //当m_sortList为空时，直接将list投入，并且设置m_splicePoint的位置
    if(m_sortList.begin() == m_sortList.end()){
        m_sortList.splice(m_sortList.end(), list);
        m_splicePoint = ++m_sortList.end();
        m_spliceCount = m_sortList.size();
        return;
    }

    //插入排序
    auto _index = m_sortList.begin();
    for(auto it = list.begin(); it != list.end();){
        while(_index != m_sortList.end() && (*it)->evTime() > (*_index)->evTime()){
            _index++;
        }
        auto _insertPoint = it;//用于标记插入点的ietrator
        it++;
        m_sortList.splice(_index, list, _insertPoint, _insertPoint);
        m_sortListLen++;//插入后，更新m_sortList的长度
        
        //根据插入点更新切割点的位置，以及切割点距离头的位置
        update_splicePoint(_insertPoint);
    }
    return;
}

bool SortListCache::evt_tm_comp(junoread::Event*& a, junoread::Event*& b){
    if(a->evTime() < b->evTime())
        return true;
    else
        return false;
}

void SortListCache::update_splicePoint(const std::list<junoread::Event*>::iterator& insertPoint){
    if((*insertPoint)->evTime() < (*m_splicePoint)->evTime()){
        m_spliceCount++;
    }

    //更新切割点的位置
    uint64_t _fTime = (*m_sortList.begin())->evTime();
    while(m_splicePoint != ++m_sortList.end() && (*m_splicePoint)->evTime() - _fTime < m_dispRate*m_cacheTime){
        m_splicePoint++;
        m_spliceCount++;
    }
    while(m_splicePoint != m_sortList.begin() && (*m_splicePoint)->evTime() - _fTime > m_dispRate*m_cacheTime){
        m_splicePoint--;
        m_spliceCount--;
    }
    
    return;
}

bool SortListCache::check_enDsp(){//检查是否可以向m_dispList派发
    //m_sortList为空不能派发
    if(m_sortList.begin() == m_sortList.end())
        return false;
    
    uint64_t _fTime = (*m_sortList.begin())->evTime();
    uint64_t _lTime = (*m_sortList.rbegin())->evTime();
    if(_lTime - _fTime > m_cacheTime && m_dispRate > m_spliceCount/m_sortListLen){
        return true;
    }

    return false;
}