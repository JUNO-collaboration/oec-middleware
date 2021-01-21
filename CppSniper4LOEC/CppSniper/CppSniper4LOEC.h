#ifndef CPP_SNIPER_4_LOEC_H
#define CPP_SNIPER_4_LOEC_H

#include "OEC_com/oec_com/EventDepository.h"
#include <boost/python.hpp>
#include <string>

class Task;
class LOECInputSvc;
class LOECOutputSvc;

class CppSniper4LOEC
{
    public:
        CppSniper4LOEC(const std::string& PyModule);
        virtual ~CppSniper4LOEC();

        void process(oec::simpleBuffer& input, oec::simpleBuffer& output);

    private:
        Task* m_task;
        LOECInputSvc*  m_input;
        LOECOutputSvc* m_output;

        boost::python::object m_pyTask;
};

#endif
