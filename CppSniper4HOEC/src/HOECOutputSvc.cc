#include "HOECOutputSvc.h"
#include "HOECNavBuf.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"
#include "TFile.h"

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

    eventTree = new TTree("oecEvent", "info about oecEvent");
    eventTree->Branch("l1id",&event.l1id,"l1id/i");
    eventTree->Branch("eventID", &event.eventID, "eventID/I");
    eventTree->Branch("tag", &event.tag, "tag/I");
    eventTree->Branch("energy", &event.energy, "energy/F");
    eventTree->Branch("vertexX", &event.vertexX, "vertexX/F");
    eventTree->Branch("vertexY", &event.vertexY, "vertexY/F");
    eventTree->Branch("vertexZ", &event.vertexZ, "vertexZ/F");

    LogInfo << "HOECOutputSvc initialized successfully" << std::endl;

    return true;
}

bool HOECOutputSvc::finalize()
{   
    LogInfo<<"********************Start the finalize() of HOECOutputSvc***********************"<<std::endl;
    TFile* f = new TFile("oecEvent.root","recreate");
    eventTree->Write();
    f->Close();

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

    //FIXME: used to fill a tree to debug
    fillEvent(oecHeader, oecEvent);

    return true;
}

//FIXME: used to fill a tree to debug
void HOECOutputSvc::fillEvent(JM::OECHeader* oecHeader,JM::OECEvent* oecEvent){
    event.l1id = oecHeader->l1id();
    event.eventID =  oecHeader->EventID();
    event.tag =  oecEvent->getTag();
    event.energy = oecEvent->getEnergy();
    event.vertexX = oecEvent->getVertexX();
    event.vertexY = oecEvent->getVertexY();
    event.vertexZ = oecEvent->getVertexZ();

    eventTree->Fill();

    return;
}