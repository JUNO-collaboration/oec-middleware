#ifndef SNIPER_HOEC_EVT_MEM_MGR_H
#define SNIPER_HOEC_EVT_MEM_MGR_H

//#include "BufferMemMgr/IDataMemMgr.h"
#include "SniperKernel/SvcBase.h"

class HOECEvtMemMgr : public SvcBase //, public IDataMemMgr
{
    public :

        HOECEvtMemMgr(const std::string& name);

        virtual ~HOECEvtMemMgr();

        bool initialize();
        bool finalize();

        //bool adopt(JM::EvtNavigator* nav, const std::string& path);

        //bool reset(const std::string& path, int entry);

    private :
        std::vector<double>  m_bufBounds;
};

#endif
