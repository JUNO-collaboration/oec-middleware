#ifndef HOEC_PROCESSOR_H
#define HOEC_PROCESSOR_H

#include "oec_com/AlgInterface.h"
#include <cstdint>
#include <list>
#include <mutex>
#include <condition_variable>
#include "SortListCache.h"

class CppSniper4HOEC;
class RecEvt;

class HOECProcessor : public oec::AlgInterface
{
public:
    HOECProcessor();
    virtual ~HOECProcessor();

    virtual void oec_process(void* event, void* results);
    
private:
    SortListCache m_orderedQueue;

    CppSniper4HOEC* m_hoec;   //for high level OEC in SNiPER

};


#endif
