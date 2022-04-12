#include "HOECProcessor.h"
#include "CppSniper/CppSniper4HOEC.h"
#include <cstdint>
#include <iostream>
#include <unistd.h> 

HOECProcessor::HOECProcessor()
{   std::cout<<"#######################Create a HOECProcessor"<<std::endl;
    m_hoec = new CppSniper4HOEC("HOECConfig");
}

HOECProcessor::~HOECProcessor()
{
    delete m_hoec;
}

void HOECProcessor::oec_process(void* input, void* results)
{
    std::cout<<"This is ES of Sniper"<<std::endl;

    const std::vector<oec::OECRecEvt*>& evtVec = *(reinterpret_cast<std::vector<oec::OECRecEvt*>*>(input));
    m_orderedQueue.enqueue(evtVec);
    std::cout<<"#####################Successfully put evt in sortList########"<<std::endl;
    sleep(1);
    while(true){
        oec::OECRecEvt* event = m_orderedQueue.get_evt();
        if(event == nullptr) break;
        m_hoec->process(event, results);
    }
}


extern "C" {
    oec::AlgInterface *create_processor()
    {
        return new HOECProcessor;
    }
}
