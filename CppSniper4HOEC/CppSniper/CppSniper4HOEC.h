#ifndef CPP_SNIPER_4_HOEC_H
#define CPP_SNIPER_4_HOEC_H

#include "oec_com/OEC_define.h"
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

        oec::OECRecEvt* process(oec::OECRecEvt* vertex_ofone_ev);

    private:
        Task* m_task;
        HOECInputSvc*  m_input;
        HOECOutputSvc* m_output;

        //boost::python::object m_pyTask;

        std::deque<oec::OECRecEvt*> m_recEvts;
        

        //为了让处理完的事例以时间片为单位集中返还，而用到的数据结构
        //struct BufAndEvts {
        //    int count;
        //    oec::simpleBuffer buf;
        //    std::deque<oec::OECRecEvt*> evts;
        //};
        //std::map<uint32_t, BufAndEvts> m_dataMap;
};

#endif
