#include "OecEvtConverter/OecEvtConverter.h"
#include "SniperKernel/SniperLog.h"

OecEvtConverter::OecEvtConverter(uint8_t* startPtr):
m_startPtr(startPtr)
{}

uint32_t OecEvtConverter::writeOecEvt(JM::EvtNavigator* nav, uint32_t l1id){
    auto oecHeader = dynamic_cast<JM::OecHeader*>(nav->getHeader("/Event/Oec"));
    if ( ! oecHeader ) {
        LogFatal << "Failed to get OecHeader" << std::endl;
        return false;
    }
    auto oecEvent = (JM::OecEvt*)oecHeader->event("JM::OecEvt");

    uint8_t* nib = m_startPtr;
    oec::OECRecEvt* oecStruct = (oec::OECRecEvt*)nib;
    const auto& time = nav->TimeStamp();
    oecStruct->marker = oec::Vertex_start_marker;
    oecStruct->l1id = l1id;
    oecStruct->evtId = nav->EventID();
    oecStruct->sec = time.GetSec();
    oecStruct->nanoSec = time.GetNanoSec();
    oecStruct->nCluster = 0; //not used yet
    oecStruct->tag = oecEvent->getTag();
    oecStruct->energy = oecEvent->getEnergy();
    oecStruct->x = oecEvent->getVertexX();
    oecStruct->y = oecEvent->getVertexY();
    oecStruct->z = oecEvent->getVertexZ();
    oecStruct->qbf = oecEvent->getQBF();
    nib += sizeof(oec::OECRecEvt);

    nib += writeArray(nib, oecEvent->getMuID());
    nib += writeArray(nib, oecEvent->getMuInX());
    nib += writeArray(nib, oecEvent->getMuInY());
    nib += writeArray(nib, oecEvent->getMuInZ());
    nib += writeArray(nib, oecEvent->getMuOutX());
    nib += writeArray(nib, oecEvent->getMuOutY());
    nib += writeArray(nib, oecEvent->getMuOutZ());

    uint32_t* endMarker = (uint32_t*)nib;
    *endMarker = oec::Vertex_end_marker;
    nib += sizeof(uint32_t);

    oecStruct->totalSize = static_cast<uint32_t>(nib - m_startPtr);
    return oecStruct->totalSize;
}

JM::EvtNavigator* OecEvtConverter::getOecEvt(){
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    JM::OecHeader* oecHeader = new JM::OecHeader();
    nav->addHeader("/Event/Oec", oecHeader);
    JM::OecEvt* oecEvent = new JM::OecEvt();
    oecHeader->setEvent(oecEvent);

    uint8_t* nib = (uint8_t*)m_startPtr;
    oec::OECRecEvt* oecStruct = (oec::OECRecEvt*)nib;
    assert(oecStruct->marker == oec::Vertex_start_marker);
    nav->setTimeStamp(TTimeStamp((time_t)(oecStruct->sec), (Int_t)(oecStruct->nanoSec)));
    nav->setEventID(oecStruct->evtId);
    oecHeader->setL1id(oecStruct->l1id);
    oecEvent->setTime(TTimeStamp((time_t)(oecStruct->sec), (Int_t)(oecStruct->nanoSec)));
    oecEvent->setQBF(oecStruct->qbf);
    oecEvent->addTag(oecStruct->tag);
    oecEvent->setEnergy(oecStruct->energy);
    oecEvent->setVertexX(oecStruct->x);
    oecEvent->setVertexY(oecStruct->y);
    oecEvent->setVertexZ(oecStruct->z);
    nib += sizeof(oec::OECRecEvt);

    std::vector<int> muid = getArray<int>(nib);
    nib += sizeof(int) * muid.size() + sizeof(uint32_t) + sizeof(uint32_t);
    std::vector<float> muinx = getArray<float>(nib);
    nib += sizeof(float) * muinx.size() + sizeof(uint32_t) + sizeof(uint32_t);
    std::vector<float> muiny = getArray<float>(nib);
    nib += sizeof(float) * muiny.size() + sizeof(uint32_t) + sizeof(uint32_t);
    std::vector<float> muinz = getArray<float>(nib);
    nib += sizeof(float) * muinz.size() + sizeof(uint32_t) + sizeof(uint32_t);
    std::vector<float> muoutx = getArray<float>(nib);
    nib += sizeof(float) * muoutx.size() + sizeof(uint32_t) + sizeof(uint32_t);
    std::vector<float> muouty = getArray<float>(nib);
    nib += sizeof(float) * muouty.size() + sizeof(uint32_t) + sizeof(uint32_t);
    std::vector<float> muoutz = getArray<float>(nib);
    nib += sizeof(float) * muoutz.size() + sizeof(uint32_t) + sizeof(uint32_t);
    
    uint32_t* endMarker = (uint32_t*)nib;
    assert(*endMarker == oec::Vertex_end_marker);
    nib += sizeof(uint32_t);
    assert((nib - m_startPtr) == oecStruct->totalSize);

    oecEvent->setMuID(muid);
    oecEvent->setMuInX(muinx);
    oecEvent->setMuInY(muiny);
    oecEvent->setMuInZ(muinz);
    oecEvent->setMuOutX(muoutx);
    oecEvent->setMuOutY(muouty);
    oecEvent->setMuOutZ(muoutz);

    return nav;
}

