#ifndef LOEC_INPUT_SVC_H
#define LOEC_INPUT_SVC_H

#include "SniperKernel/SvcBase.h"
#include "OEC_com/oec_com/EventDepository.h"

class LOECNavBuf;

class LOECInputSvc : public SvcBase
{
    public :

        LOECInputSvc(const std::string& name);
        virtual ~LOECInputSvc();

        bool initialize();
        bool finalize();

        inline bool get(oec::simpleBuffer& input) {
            return (this->*m_get)(input);
        }

    private :

        bool getWaveform(oec::simpleBuffer& wfEvt);
        bool getQT(oec::simpleBuffer& qtEvt);

        bool (LOECInputSvc::*m_get)(oec::simpleBuffer& evt);

        LOECNavBuf* m_buf;
        std::string m_type;
};

#endif
