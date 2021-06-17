#include "LOECEvtMemMgr.h"
#include "LOECNavBuf.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(LOECEvtMemMgr);

LOECEvtMemMgr::LOECEvtMemMgr(const std::string& name)
    : SvcBase(name)
{
}

LOECEvtMemMgr::~LOECEvtMemMgr()
{
}

bool LOECEvtMemMgr::initialize()
{
    SniperPtr<DataMemSvc> mSvc(m_par, "DataMemSvc");
    mSvc->regist("/Event", new LOECNavBuf());

    return true;
}

bool LOECEvtMemMgr::finalize()
{
    return true;
}
