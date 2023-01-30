#include "FragmentRingArray.h"
#include <cassert>
#include "SniperKernel/SniperLog.h"

using namespace std;

FragmentRingArray::Iterator::Iterator(FragmentRingArray& ringArray){
    ptr = ringArray.m_array;
    arrayLen = ringArray.m_arrayLen;
    locate = 0;
}

FragmentRingArray::Iterator& FragmentRingArray::Iterator::operator++(){
    locate++;
    locate = locate % arrayLen;
    return *this;
}

HOECFragment& FragmentRingArray::Iterator::operator*()const{
    return ptr[locate];
}

HOECFragment* FragmentRingArray::Iterator::operator->()const{
    return &(this->operator*());
}

FragmentRingArray::FragmentRingArray(int length)
{   
    m_arrayLen = length;
    m_array = new HOECFragment[m_arrayLen];
}

FragmentRingArray::~FragmentRingArray()
{
    delete [] m_array;
}

int FragmentRingArray::insertFrag(std::shared_ptr<std::vector<void*>> evtsPtr, uint32_t l1id, uint32_t timeSec, uint16_t nanoSec){
    int _locate = (int)(l1id % (uint32_t)m_arrayLen);
    HOECFragment& _frag = m_array[_locate];
    if(_frag.stat != HOECFragment::Status::empty && _frag.stat != HOECFragment::Status::late){
        LogError<<"failed to insert. the stat is "<<_frag.stat<<" locate: "<<_locate<<std::endl;
        snapShot();
        assert(_frag.stat == HOECFragment::Status::empty || _frag.stat == HOECFragment::Status::late);
    }
    _frag.evtsPtr = evtsPtr;
    _frag.l1id = l1id;
    _frag.timeSec = timeSec;
    _frag.nanoSec = nanoSec;
    _frag.stat = HOECFragment::Status::ready;

    return _locate;
}

bool FragmentRingArray::cleanFrag(FragmentRingArray::Iterator it){
    assert(it->stat != HOECFragment::Status::empty);
    it->evtsPtr = nullptr;
    it->stat = HOECFragment::Status::empty;
    return true;
}

HOECFragment& FragmentRingArray::operator[](int locate){
    assert(locate < m_arrayLen);
    return m_array[locate];
}

void FragmentRingArray::snapShot(){
    LogError<<"Something unexpected happens. Here is the snapshot of FragmentRingArray"<<std::endl;
    for(int i = 0; i < m_arrayLen; i++){
        std::string stat = " unknow";
        switch(m_array[i].stat){
            case HOECFragment::Status::empty: stat = " empty";
            case HOECFragment::Status::ready: stat = " ready";
            case HOECFragment::Status::late: stat = " late";
            case HOECFragment::Status::inWorker: stat = " inWorker";
            case HOECFragment::Status::returned: stat = " returned";
        }

        LogError<<std::dec<<"Locate: "<<i<<" TFid: "<<m_array[i].l1id<<" Status code "<<m_array[i].stat<<stat<<" "<<m_array[i].timeSec<<" "<<m_array[i].nanoSec<<std::endl;
    }
}



