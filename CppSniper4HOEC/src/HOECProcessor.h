#ifndef HOEC_PROCESSOR_H
#define HOEC_PROCESSOR_H

#include "OEC_com/oec_com/AlgInterface.h"
#include <cstdint>

class CppSniper4HOEC;

class HOECProcessor : public oec::AlgInterface
{
public:
    HOECProcessor();
    virtual ~HOECProcessor();

    virtual void oec_process(void* event, void* results);
    
private:
    CppSniper4HOEC* m_hoec;   //for high level OEC in SNiPER
};
#endif
