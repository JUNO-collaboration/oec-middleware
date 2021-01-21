#ifndef LOEC_OUTPUT_SVC_H
#define LOEC_OUTPUT_SVC_H

#include "SniperKernel/SvcBase.h"
#include "OEC_com/oec_com/EventDepository.h"

class LOECNavBuf;

class LOECOutputSvc : public SvcBase
{
    public :

        LOECOutputSvc(const std::string& name);
        virtual ~LOECOutputSvc();

        bool initialize();
        bool finalize();

        bool put(oec::simpleBuffer& output);

    private :

        bool putQT(oec::simpleBuffer& qtEvt);
        bool putVertex(oec::simpleBuffer& vtxEvt);

        bool (LOECOutputSvc::*m_put)(oec::simpleBuffer& evt);

        LOECNavBuf* m_buf;
        std::string m_type;
};

#endif
