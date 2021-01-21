#ifndef LOEC_PROCESSOR_H
#define LOEC_PROCESSOR_H

#include "OEC_com/oec_com/AlgInterface.h"
#include <cstdint>

class CppSniper4LOEC;

class LOECProcessor : public oec::AlgInterface
{
public:
    LOECProcessor();
    virtual ~LOECProcessor();

    virtual void oec_process(void* event, void* /*nullptr*/);
    
private:
    int m_nhit = 30;
    uint64_t m_trgWin = 200; //200ns 时间窗口
    uint64_t m_dataWin = 1500; //1us

    CppSniper4LOEC* m_wfRec;   //for waveform reconstruction
    CppSniper4LOEC* m_vtxRec;  //for vertex reconstruction
};

#endif
