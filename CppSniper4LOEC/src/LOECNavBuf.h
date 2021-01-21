#ifndef LOEC_NAV_BUF_H
#define LOEC_NAV_BUF_H

#include "EvtNavigator/NavBuffer.h"

class LOECNavBuf : public JM::NavBuffer
{
    public :

        LOECNavBuf();
        virtual ~LOECNavBuf();

        void set(JM::EvtNavigator* evt);
        void clear();

        uint32_t l1id;
};

#endif
