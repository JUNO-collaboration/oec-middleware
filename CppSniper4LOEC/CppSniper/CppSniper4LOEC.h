#ifndef CPP_SNIPER_4_LOEC_H
#define CPP_SNIPER_4_LOEC_H

#include "OEC_com/oec_com/EventDepository.h"
//#include <boost/python.hpp>
#include <vector>
#include <string>

#include <boost/thread/mutex.hpp>

class Task;
class LOECInputSvc;
class LOECOutputSvc;

class CppSniper4LOEC 
{
    public:
        CppSniper4LOEC(const std::string& PyModule);
        virtual ~CppSniper4LOEC();
        void initialize();
        virtual void process(oec::EventDepository* );

    private:
        Task* m_task;
        LOECInputSvc*  m_input;
        LOECOutputSvc* m_output;

        //boost::python::object m_pyTask;

        static boost::mutex cppSniperMutex;

        static int waveTaskNum;
};

#endif
