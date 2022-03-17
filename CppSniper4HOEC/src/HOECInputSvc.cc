#include "HOECNavBuf.h"
#include "HOECInputSvc.h"
#include "Event/OECHeader.h"
#include "EvtNavigator/EvtNavigator.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(HOECInputSvc);

HOECInputSvc::HOECInputSvc(const std::string& name)
    : SvcBase(name)
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
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    m_buf->set(nav);

    JM::OECHeader* header = new JM::OECHeader();
    nav->addHeader("/Event/OEC", header);

    JM::OECEvent* event = new JM::OECEvent();
    header->setEvent(event);

    //convert DAQ OECRecEvt to offline OECEvent
    nav->setTimeStamp(TTimeStamp(evt->sec, evt->nanoSec));
    header->setL1id(evt->l1id);
    header->setEventID(evt->evtId);
    event->setTime(TTimeStamp(evt->sec, evt->nanoSec));
    event->addTag(evt->tag);
    event->setEnergy(evt->energy);
    event->setVertexX(evt->x);
    event->setVertexY(evt->y);
    event->setVertexZ(evt->z);
    event->setMuInX(evt->muinx);
    event->setMuInY(evt->muiny);
    event->setMuInZ(evt->muinz);
    event->setMuOutX(evt->muoutx);
    event->setMuOutY(evt->muouty);
    event->setMuOutZ(evt->muoutz);
    event->setMuID(evt->muid);

    LogInfo << " L1ID: " << header->l1id()
        << " EventID: " << header->EventID()
        << " TAG: 0x" << std::hex << event->getTag() << std::dec
        << " time: " << nav->TimeStamp()
        << " energy: " << event->getEnergy()
        << " vertex: [" << event->getVertexX()
        << ", " << event->getVertexY()
        << ", " << event->getVertexZ()
        << "]" << std::endl;

    return true;
}

bool HOECInputSvc::next()
{
    return m_buf->next();
}
