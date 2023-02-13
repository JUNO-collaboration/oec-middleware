#ifndef HOEC_INPUT_SVC_H
#define HOEC_INPUT_SVC_H

#include "SniperKernel/SvcBase.h"
#include "oec_com/OEC_define.h"
#include "OecEvtConverter/OecEvtConverter.h"

class HOECNavBuf;

class HOECInputSvc : public SvcBase
{
    public :

        HOECInputSvc(const std::string& name);
        virtual ~HOECInputSvc();

        bool initialize();
        bool finalize();

        bool get(oec::OECRecEvt* evt);
        bool next();

        uint32_t m_l1id;

    private :
        OecEvtConverter* m_converter;
        HOECNavBuf* m_buf;
};

#endif
