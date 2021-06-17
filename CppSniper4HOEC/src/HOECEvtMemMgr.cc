#include "HOECEvtMemMgr.h"
#include "HOECNavBuf.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(HOECEvtMemMgr);

HOECEvtMemMgr::HOECEvtMemMgr(const std::string& name)
    : SvcBase(name)
{
    declProp("TimeWindow", m_bufBounds);
}

HOECEvtMemMgr::~HOECEvtMemMgr()
{
}

bool HOECEvtMemMgr::initialize()
{
    if ( m_bufBounds.size() != 2 ) {
        m_bufBounds.resize(2);
        m_bufBounds[0] = -1.0, m_bufBounds[1] = 1.0;
    }
    SniperPtr<DataMemSvc> mSvc(m_par, "DataMemSvc");
    mSvc->regist("/Event", new HOECNavBuf(m_bufBounds[0], m_bufBounds[1]));

    return true;
}

bool HOECEvtMemMgr::finalize()
{
    return true;
}
