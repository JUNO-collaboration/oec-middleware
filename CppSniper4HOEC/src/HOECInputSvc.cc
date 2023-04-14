#include "HOECNavBuf.h"
#include "HOECInputSvc.h"
#include "Event/OecHeader.h"
#include "EvtNavigator/EvtNavigator.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(HOECInputSvc);

HOECInputSvc::HOECInputSvc(const std::string& name)
    : SvcBase(name), m_converter(nullptr)
{
}

HOECInputSvc::~HOECInputSvc()
{
}

bool HOECInputSvc::initialize()
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

bool HOECInputSvc::finalize()
{
    return true;
}

bool HOECInputSvc::get(oec::OECRecEvt* evt)
{
    //for test:delete later
    assert(evt->marker == 0x12345678);
    m_converter.setStartPtr((uint8_t*)evt);
    auto nav = m_converter.getOecEvt();
    m_buf->set(nav);

    return true;
}

bool HOECInputSvc::next()
{
    return m_buf->next();
}
