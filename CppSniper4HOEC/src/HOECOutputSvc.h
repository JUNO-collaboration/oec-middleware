#ifndef HOEC_OUTPUT_SVC_H
#define HOEC_OUTPUT_SVC_H

#include "SniperKernel/SvcBase.h"

class HOECNavBuf;

class HOECOutputSvc : public SvcBase
{
    public :

        HOECOutputSvc(const std::string& name);
        virtual ~HOECOutputSvc();

        bool initialize();
        bool finalize();

        bool put(uint32_t& l1id, uint32_t& tag);

    private :

        HOECNavBuf* m_buf;
};

#endif
