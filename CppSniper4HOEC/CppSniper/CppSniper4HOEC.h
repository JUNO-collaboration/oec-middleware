#ifndef CPP_SNIPER_4_HOEC_H
#define CPP_SNIPER_4_HOEC_H

#include "OEC_com/oec_com/EventDepository.h"
#include "OEC_com/oec_com/OEC_define.h"
//#include <boost/python.hpp>
#include <deque>
#include <string>

class Task;
class HOECInputSvc;
class HOECOutputSvc;

class CppSniper4HOEC
{
    public:
        CppSniper4HOEC(const std::string& PyModule);
        virtual ~CppSniper4HOEC();

        void process(oec::simpleBuffer* input, oec::EventDepository* output);

    private:
        Task* m_task;
        HOECInputSvc*  m_input;
        HOECOutputSvc* m_output;

        //boost::python::object m_pyTask;

        struct BufAndEvts {
            int count;
            oec::simpleBuffer buf;
            std::deque<oec::OECRecEvt*> evts;
        };
        std::map<uint32_t, BufAndEvts> m_dataMap;
};

#endif
