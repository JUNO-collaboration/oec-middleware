#include "CppSniper/CppSniper4LOEC.h"
#include "LOECInputSvc.h"
#include "LOECOutputSvc.h"
#include "SniperKernel/Task.h"

#include <Python.h>
#include "OEC_com/oec_com/OEC_define.h"
#include "OEC_com/oec_com/pack.h"
#include "SniperKernel/SniperLog.h"

#include <iostream>

int CppSniper4LOEC::waveTaskNum = 0;

CppSniper4LOEC::CppSniper4LOEC(boost::mutex& cppSniperMutex, const std::string& PyModule = "LOECWaveformRec")
{   
    
    
    cppSniperMutex.lock();
    LogInfo << "Start to Py_IsInitialize" << std::endl;
    if ( ! Py_IsInitialized() ) {
        Py_Initialize();
    }
    LogInfo << "Start to create pytask" << std::endl;
    boost::python::object config = boost::python::import(PyModule.c_str());
    waveTaskNum++;
    m_pyTask = config.attr("GetTask")(waveTaskNum);

    std::cout << "To get Cpp task" << std::endl;
    m_task = boost::python::extract<Task*>(m_pyTask);
    m_task->Snoopy().config();
    m_task->Snoopy().initialize();
    cppSniperMutex.unlock();
    //m_input = dynamic_cast<IOECInputSvc*>(m_task->find("InputSvc"));
    m_input = dynamic_cast<LOECInputSvc*>(m_task->find("InputSvc"));
    m_output = dynamic_cast<LOECOutputSvc*>(m_task->find("OutputSvc"));

    
    

    if ( PyModule == "LOECWaveformRec" ) {
        //waveform reconstruction
        m_itag = STREAM_TAG_WAVE;
        m_otag = STREAM_TAG_WAVE_TQ;
    }
    else if ( PyModule == "LOECVertexRec" ) {
        //vertex reconstruction
        m_itag = STREAM_TAG_WAVE_TQ;
        m_otag = EVENTS_VERTEX_WAVE;
    }
    else {
        //Wrong ...
    }
}

CppSniper4LOEC::~CppSniper4LOEC()
{
    m_task->Snoopy().finalize();

    // this must not be invoked by current version of boost.python
    // Py_Finalize();
    // may be fixed in a future version
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

