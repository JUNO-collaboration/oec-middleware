#include "CppSniper/CppSniper4LOEC.h"
#include "LOECInputSvc.h"
#include "LOECOutputSvc.h"

#include "OEC_com/oec_com/OEC_define.h"
#include "OEC_com/oec_com/pack.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperLog.h"

#include <iostream>

int CppSniper4LOEC::waveTaskNum = 0;
boost::mutex CppSniper4LOEC::cppSniperMutex;
CppSniper4LOEC::CppSniper4LOEC(const std::string& PyModule = "LOECWaveformRec")
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

void CppSniper4LOEC:: process(oec::EventDepository* one_eventDepo)
{

    //std::cout<<"Hello! This is ps."<<std::endl;
    
    auto& evt_map = one_eventDepo->getOneEventSimpleBufferMap();
    auto temp = evt_map[STREAM_TAG_WAVE];
    //std::cout<<"Accessing DAQ data..."<<std::endl;
    m_input->getWaveform(evt_map[STREAM_TAG_WAVE]);

    //std::cout<<" rec start"<<std::endl;
    m_task->Snoopy().run_once();
    //std::cout<<" rec end"<<std::endl;
    
    //std::cout<<" Put QT"<<std::endl;
    m_output->putQT(evt_map[STREAM_TAG_WAVE_TQ]);

    //std::cout<<" Put Vertex"<<std::endl;
    m_output->putVertex(evt_map[EVENTS_VERTEX_WAVE]);
    std::cout<<"data has been sent to oec"<<std::endl;
    
    m_output->clear();
}

