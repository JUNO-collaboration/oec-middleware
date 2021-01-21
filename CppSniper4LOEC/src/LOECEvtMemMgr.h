#ifndef SNIPER_LOEC_EVT_MEM_MGR_H
#define SNIPER_LOEC_EVT_MEM_MGR_H

//#include "BufferMemMgr/IDataMemMgr.h"
#include "SniperKernel/SvcBase.h"

class LOECEvtMemMgr : public SvcBase //, public IDataMemMgr
{
    public :

        LOECEvtMemMgr(const std::string& name);

        virtual ~LOECEvtMemMgr();

        bool initialize();
        bool finalize();

        //bool adopt(JM::EvtNavigator* nav, const std::string& path);

        //bool reset(const std::string& path, int entry);

    private :

};

#endif
