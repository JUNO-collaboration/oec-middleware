#include "HOECNavBuf.h"
#include "HOECInputSvc.h"
#include "Event/OecHeader.h"
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

    JM::OecHeader* header = new JM::OecHeader();
    nav->addHeader("/Event/Oec", header);

    JM::OecEvt* event = new JM::OecEvt();
    header->setEvent(event);

    //for test:delete later
    assert(evt->marker == 0x12345678);
    std::cout<<"evt amrker "<<hex<<evt->marker<<std::endl;
    std::cout<<"The secod is "<<evt->sec<<" nano second is "<<evt->nanoSec<<std::endl;
    

    //convert DAQ OECRecEvt to offline OecEvt
    nav->setTimeStamp(TTimeStamp((time_t)(evt->sec), (Int_t)(evt->nanoSec)));
    header->setL1id(evt->l1id);
    nav->setEventID(evt->evtId);
    event->setTime(TTimeStamp((time_t)(evt->sec), (Int_t)evt->nanoSec));
    event->addTag(evt->tag);
    event->setEnergy(evt->energy);
    event->setVertexX(evt->x);
    event->setVertexY(evt->y);
    event->setVertexZ(evt->z);
    //FixMe:用来转移OECEvt的接口需要重新定义 以承接新的OECEvt内容
    //event->setMuInX(evt->muinx);
    //event->setMuInY(evt->muiny);
    //event->setMuInZ(evt->muinz);
    //event->setMuOutX(evt->muoutx);
    //event->setMuOutY(evt->muouty);
    //event->setMuOutZ(evt->muoutz);
    //event->setMuID(evt->muid);

    LogInfo << " L1ID: " << header->l1id()
        << " EventID: " << nav->EventID()
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
