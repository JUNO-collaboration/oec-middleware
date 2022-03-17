#include "HOECProcessor.h"
#include "CppSniper/CppSniper4HOEC.h"
#include <cstdint>
#include <iostream>

HOECProcessor::HOECProcessor()
{   std::cout<<"Create a HOECProcessor"<<std::endl;
    m_hoec = new CppSniper4HOEC("HOECConfig");
}

HOECProcessor::~HOECProcessor()
{
    delete m_hoec;
}

void HOECProcessor::oec_process(void* event, void* results)
{
    //oec::simpleBuffer* input = reinterpret_cast<oec::simpleBuffer*>(event);
    //oec::EventDepository* output = reinterpret_cast<oec::EventDepository*>(results);

    m_hoec->process(event, results);
}


extern "C" {
    oec::AlgInterface *create_processor()
    {
        return new HOECProcessor;
    }
}
