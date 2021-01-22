#ifndef CPP_SNIPER_4_LOEC_H
#define CPP_SNIPER_4_LOEC_H

#include "OEC_com/oec_com/EventDepository.h"
#include <boost/python.hpp>
#include <vector>
#include <string>
#include "OEC_com/oec_com/AlgInterface.h"

class Task;
class LOECInputSvc;
class LOECOutputSvc;

class CppSniper4LOEC : public oec::AlgInterface
{
    public:
        CppSniper4LOEC(const std::string& PyModule);
        virtual ~CppSniper4LOEC();

        virtual void oec_process(void *, void *);

    private:
        uint32_t m_itag;
        uint32_t m_otag;

        Task* m_task;
        LOECInputSvc*  m_input;
        LOECOutputSvc* m_output;

        boost::python::object m_pyTask;
};

#endif
