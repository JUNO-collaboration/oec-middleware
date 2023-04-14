#ifndef OEC_EVT_CONVERTER_H
#define OEC_EVT_CONVERTER_H

#include "oec_com/OEC_define.h"
#include "Event/OecHeader.h"
#include "EvtNavigator/EvtNavigator.h"

class OecEvtConverter{
public:
    const uint32_t Array_header = 0x12345670;

    OecEvtConverter(uint8_t* startPtr);
    void setStartPtr(uint8_t* ptr){
        m_startPtr = ptr;}
    uint32_t writeOecEvt(JM::EvtNavigator* nav, uint32_t l1id);//返回值是写入buffer后的长度
    JM::EvtNavigator* getOecEvt();

private:
    uint8_t* m_startPtr;

    template <class T>
    uint32_t writeArray(uint8_t* ptr, const std::vector<T>& arr);

    template <class T>
    std::vector<T> getArray(uint8_t* ptr);
};

template <class T>
uint32_t OecEvtConverter::writeArray(uint8_t* ptr, const std::vector<T>& arr){
    uint8_t* nib = ptr;

    uint32_t* header = (uint32_t*)nib;
    *header = Array_header;
    nib += sizeof(uint32_t);
    
    uint32_t* arrLen = (uint32_t*)nib;
    *arrLen = static_cast<uint32_t>(arr.size());
    nib += sizeof(uint32_t);

    for(auto n : arr){
        T* num = (T*)nib;
        *num = n;
        nib += sizeof(T);        
    }
    
    assert((size_t)(nib - ptr) == sizeof(uint32_t)*2 + sizeof(T)*arr.size());

    return static_cast<uint32_t>(nib - ptr);
}

template <class T>
std::vector<T> OecEvtConverter::getArray(uint8_t* ptr){
    uint8_t* nib = ptr;

    uint32_t* header = (uint32_t*)ptr;
    assert(*header == Array_header);
    nib += sizeof(uint32_t);

    uint32_t* arrLen = (uint32_t*)nib;
    nib += sizeof(uint32_t);

    std::vector<T> vec(*arrLen);
    T* num = (T*)nib;
    for(uint32_t i = 0; i < *arrLen; i++){
        vec[i] = num[i];
    }
    nib += sizeof(T) * (*arrLen);

    return vec;
}
#endif