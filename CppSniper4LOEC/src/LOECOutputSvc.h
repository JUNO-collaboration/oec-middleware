#ifndef LOEC_OUTPUT_SVC_H
#define LOEC_OUTPUT_SVC_H

#include "SniperKernel/SvcBase.h"
#include <juno_pack/Event.h>
#include "OecEvtConverter/OecEvtConverter.h"

class LOECNavBuf;
class RootOutputSvc;

class LOECOutputSvc : public SvcBase
{
    public :

        LOECOutputSvc(const std::string& name);
        virtual ~LOECOutputSvc();

        bool initialize();
        bool finalize();
        bool putQT(junoread::Event& OnlineEvt);
        bool putVertex(junoread::Event& OnlineEvt);
        void clear();

        int         m_totalInPack;

    private :
        LOECNavBuf* m_buf;
        void*       m_cache;
        int         m_nInPack;
        OecEvtConverter* m_converter;
    
        std::string m_type;

        RootOutputSvc* m_oSvc;
};

#endif
