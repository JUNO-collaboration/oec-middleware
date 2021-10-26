#ifndef SORT_LIST_CACHE
#define SORT_LIST_CACHE

#include <list>
#include <mutex>
#include <condition_variable>
#include "juno_pack/Event.h"
#include <thread>


class SortListCache{
public:
    SortListCache(uint64_t cacheTime, float dispRate);//指定缓存多长时间的事例，每次派发的事例占缓存的比例
    ~SortListCache();
    
    void push_back(junoread::Event* const &);
    void enqueue(std::list<junoread::Event*>& input);
    void disptch(std::list<junoread::Event*>& output);
    
private:
    uint64_t m_cacheTime; 
    float m_dispRate;

    std::list<junoread::Event*> m_recvList;
    std::list<junoread::Event*> m_sortList;
    std::list<junoread::Event*> m_dispList;
    
    //以下部件服务于m_sortList
    std::list<junoread::Event*>::iterator m_splicePoint; //标记切割点的iterator
    uint32_t m_spliceCount;//表示切割点距离头的距离
    uint32_t m_sortListLen;//记录整个m_sortlist的长度

    std::mutex m_rMutex;
    std::mutex m_dMutex;
    
    std::condition_variable m_sortCondi;
    std::condition_variable m_dispCondi;

    std::thread m_sortThrd;
    
    void thread_work();
    void en_sortList();//将m_recvList中的事例转移到m_sortList中排序
    void en_dispList();

    //在en_sortList()中被调用
    void get_re_List(std::list<junoread::Event*>& list);//获取m_recvList, 暂存在list中
    void insert_sort(std::list<junoread::Event*>& list);//将list有序插入到m_sortList中
    static bool evt_tm_comp(junoread::Event*& a, junoread::Event*& b);//比较事例的时间先后，用于排序
    void update_splicePoint(const std::list<junoread::Event*>::iterator& insertPoint);//根据插入点的位置更新切割点
    
    bool check_enDsp();
};
 

#endif 