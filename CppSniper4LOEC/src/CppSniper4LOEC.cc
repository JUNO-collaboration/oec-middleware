#include "CppSniper/CppSniper4LOEC.h"
#include "LOECInputSvc.h"
#include "LOECOutputSvc.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperLog.h"
#include <juno_pack/Event.h>
#include <iostream>

int CppSniper4LOEC::waveTaskNum = 0;

CppSniper4LOEC::CppSniper4LOEC()
{   
    LogInfo << "To get Cpp task" << std::endl;

    auto fcfg = std::string{getenv("OFFLINE_DIR")} + "/config/LOECConfig.json";
    auto jtask = Sniper::eval(fcfg.c_str());
    m_task = dynamic_cast<Task*>(jtask);

    waveTaskNum++;
}

CppSniper4LOEC::~CppSniper4LOEC()
{
    m_task->Snoopy().finalize();
    delete m_task;
}

void CppSniper4LOEC:: initialize(){
    m_task->Snoopy().config();
    m_task->Snoopy().initialize();
    
    //m_input = dynamic_cast<IOECInputSvc*>(m_task->find("InputSvc"));
    m_input = dynamic_cast<LOECInputSvc*>(m_task->find("InputSvc"));
    m_output = dynamic_cast<LOECOutputSvc*>(m_task->find("OutputSvc"));
}

void CppSniper4LOEC:: process(void* evtBufPtr)
{   
    
    junoread::Event event(reinterpret_cast<uint8_t*>(evtBufPtr));
    
    m_input->getWaveform(event);
   
    m_task->Snoopy().run_once();

    m_output->putQT(event);
    m_output->putVertex(event);
    
    std::cout<<"data has been sent to oec"<<std::endl;
    
    m_output->clear();
}

