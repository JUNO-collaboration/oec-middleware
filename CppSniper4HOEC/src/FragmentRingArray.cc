#include "FragmentRingArray.h"
#include <cassert>

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
    assert(_frag.stat == HOECFragment::Status::empty);
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





