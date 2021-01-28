#include "HOECOutputSvc.h"
#include "HOECNavBuf.h"
#include "Event/OECHeader.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(HOECOutputSvc);

HOECOutputSvc::HOECOutputSvc(const std::string& name)
    : SvcBase(name)
{
}

HOECOutputSvc::~HOECOutputSvc()
{
}

bool HOECOutputSvc::initialize()
{
    SniperDataPtr<HOECNavBuf> buf(getParent(), "/Event");
    if ( buf.invalid() ) {
        LogFatal << "Cannot find HOECNavBuf" << std::endl;
        return false;
    }

    m_buf = buf.data();

    LogInfo << "initialized successfully" << std::endl;

    return true;
}

bool HOECOutputSvc::finalize()
{
    return true;
}

bool HOECOutputSvc::put(uint32_t& l1id, uint32_t& tag)
{
    JM::EvtNavigator* nav = m_buf->curEvt();
    auto oecHeader = dynamic_cast<JM::OECHeader*>(nav->getHeader("/Event/OEC"));
    auto oecEvent = (JM::OECEvent*)oecHeader->event("JM::OECEvent");

    LogInfo << " L1ID: " << oecHeader->l1id()
        << " EventID: " << oecHeader->EventID()
        << " TAG: 0x" << std::hex << oecEvent->getTag() << std::dec
        //<< " time: " << nav->TimeStamp()
        << " energy: " << oecEvent->getEnergy()
        << " vertex: [" << oecEvent->getVertexX()
        << ", " << oecEvent->getVertexY()
        << ", " << oecEvent->getVertexZ()
        << "]" << std::endl;

    //set result
    l1id = oecHeader->l1id();
    tag = oecEvent->getTag();

    return true;
}
