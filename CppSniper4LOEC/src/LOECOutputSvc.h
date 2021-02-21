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
        bool putQT(oec::simpleBuffer& qtEvt);
        bool putVertex(oec::simpleBuffer& vtxEvt);
        void clear();

        int         m_totalInPack;

    private :



        LOECNavBuf* m_buf;
        void*       m_cache;
        int         m_nInPack;

        std::string m_type;
};

#endif
