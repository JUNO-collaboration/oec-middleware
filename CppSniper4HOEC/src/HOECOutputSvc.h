#ifndef HOEC_OUTPUT_SVC_H
#define HOEC_OUTPUT_SVC_H

#include "SniperKernel/SvcBase.h"
#include "TTree.h"
#include "Event/OECHeader.h"

class HOECNavBuf;

class HOECOutputSvc : public SvcBase
{
    public :

        HOECOutputSvc(const std::string& name);
        virtual ~HOECOutputSvc();

        bool initialize();
        bool finalize();

        bool put(uint32_t& l1id, uint32_t& tag);

        TTree* eventTree;
        struct EventFill{
            uint32_t l1id;
            int eventID;
            int tag;
            float energy;
            float vertexX;
            float vertexY;
            float vertexZ;
        };
        EventFill event;
        
        //FIXME: used to fill a tree to debug
        void fillEvent(JM::OECHeader* oecHeader,JM::OECEvent* oecEvent);

    private :

        HOECNavBuf* m_buf;
};

#endif
