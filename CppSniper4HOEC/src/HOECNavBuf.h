#ifndef HOEC_NAV_BUF_H
#define HOEC_NAV_BUF_H

#include "EvtNavigator/NavBuffer.h"

class HOECNavBuf : public JM::NavBuffer
{
    public :

        HOECNavBuf(double lowBound, double highBound);
        virtual ~HOECNavBuf();

        void set(JM::EvtNavigator* nav) {
            m_beyond = nav;
        }

        bool next();

    private :

        void trimDated();
        double timeShift(JM::EvtNavigator* nav);

        const double       m_floor;
        const double       m_ceiling;
        JM::EvtNavigator*  m_beyond;
};

#endif
