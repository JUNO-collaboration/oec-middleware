#ifndef LOEC_INPUT_SVC_H
#define LOEC_INPUT_SVC_H

#include "SniperKernel/SvcBase.h"
#include <juno_pack/Event.h>

class LOECNavBuf;

class LOECInputSvc : public SvcBase
{
    public :

        LOECInputSvc(const std::string& name);
        virtual ~LOECInputSvc();

        bool initialize();
        bool finalize();
        bool getWaveform( junoread::Event& OnlineEvt);
    private :

        LOECNavBuf* m_buf;
        std::string m_type;
};

#endif
