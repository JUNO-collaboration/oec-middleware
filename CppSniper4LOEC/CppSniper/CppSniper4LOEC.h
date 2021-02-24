#ifndef CPP_SNIPER_4_LOEC_H
#define CPP_SNIPER_4_LOEC_H

#include "OEC_com/oec_com/EventDepository.h"
#include <boost/python.hpp>
#include <vector>
#include <string>

#include <boost/thread/mutex.hpp>

class Task;
class LOECInputSvc;
class LOECOutputSvc;

class CppSniper4LOEC 
{
    public:
        CppSniper4LOEC(boost::mutex& cppSniperMutex, const std::string& PyModule);
        virtual ~CppSniper4LOEC();

        virtual void process(oec::EventDepository* );

    private:
        uint32_t m_itag;
        uint32_t m_otag;

        Task* m_task;
        LOECInputSvc*  m_input;
        LOECOutputSvc* m_output;

        boost::python::object m_pyTask;

        static int waveTaskNum;
};

#endif
