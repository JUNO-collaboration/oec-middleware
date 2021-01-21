#ifndef DUMP_TREE_ALG_H
#define DUMP_TREE_ALG_H

#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"

class TTree;

class DumpTreeAlg : public AlgBase
{
    public:
        DumpTreeAlg(const std::string& name);
        virtual ~DumpTreeAlg();

        virtual bool initialize();
        virtual bool execute();
        virtual bool finalize();

    private:
        JM::NavBuffer* m_buf;

        TTree* m_tree;
};

#endif
