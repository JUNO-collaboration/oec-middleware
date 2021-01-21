#include "CppSniper/CppSniper4LOEC.h"
#include "LOECInputSvc.h"
#include "LOECOutputSvc.h"
#include "OEC_com/oec_com/OEC_define.h"
#include "SniperKernel/Task.h"
#include <Python.h>

CppSniper4LOEC::CppSniper4LOEC(const std::string& PyModule)
{
    if ( ! Py_IsInitialized() ) {
        Py_Initialize();
    }
    boost::python::object config = boost::python::import(PyModule.c_str());
    m_pyTask = config.attr("GetTask")();

    m_task = boost::python::extract<Task*>(m_pyTask);
    m_task->Snoopy().config();
    m_task->Snoopy().initialize();
    //m_task->show(0);

    m_input = dynamic_cast<LOECInputSvc*>(m_task->find("InputSvc"));
    m_output = dynamic_cast<LOECOutputSvc*>(m_task->find("OutputSvc"));
}

CppSniper4LOEC::~CppSniper4LOEC()
{
    m_task->Snoopy().finalize();

    // this must not be invoked by current version of boost.python
    // Py_Finalize();
    // may be fixed in a future version
}

void CppSniper4LOEC::process(oec::simpleBuffer& input, oec::simpleBuffer& output)
{
    m_input->get(input);
    m_task->Snoopy().run_once();
    m_output->put(output);
}
