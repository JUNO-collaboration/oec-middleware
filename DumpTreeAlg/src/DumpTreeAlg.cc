#include "DumpTreeAlg.h"
#include "Event/ElecHeader.h"
#include "Event/CalibHeader.h"
#include "RootWriter/RootWriter.h"
#include "SniperKernel/AlgFactory.h"
#include "Identifier/CdID.h"
#include "TTree.h"

DECLARE_ALGORITHM(DumpTreeAlg);

static Int_t EventNo = -1;
static Int_t Second  = -1;
static Int_t NanoSec = -1;
//static std::map<UShort_t, std::vector<Short_t>> Waveforms;
static std::vector<UShort_t> WFPmtIDs;
static std::vector<std::vector<Short_t>> Waveforms;
static std::vector<UShort_t> TQPmtIDs;
static std::vector<float> Time;
static std::vector<float> Charge;

DumpTreeAlg::DumpTreeAlg(const std::string& name)
    : AlgBase(name)
{
}

DumpTreeAlg::~DumpTreeAlg()
{
}

bool DumpTreeAlg::initialize()
{
    //book the plane TTree with RootWriter
    SniperPtr<RootWriter> rWriter(this->getParent(), "RootWriter");
    if ( rWriter.invalid() ) {
	LogError << "Failed to get RootWriter instance!" << std::endl;
	return false;
    }

    m_tree = new TTree("event", "simulated events");
    m_tree->Branch("EventNo",   &EventNo, "EventNo/I");
    m_tree->Branch("Second",    &Second,  "Second/I");
    m_tree->Branch("NanoSec",   &NanoSec, "NanoSec/I");
    m_tree->Branch("WFPmtIDs",  &WFPmtIDs);
    m_tree->Branch("Waveforms", &Waveforms);
    m_tree->Branch("TQPmtIDs",  &TQPmtIDs);
    m_tree->Branch("Time",      &Time);
    m_tree->Branch("Charge",    &Charge);
    rWriter->attach("FILE", m_tree);

    //get the instance of NavBuffer
    SniperDataPtr<JM::NavBuffer>  navBuf(getParent(), "/Event");
    if ( navBuf.invalid() ) {
        LogError << "cannot get the NavBuffer @ /Event" << std::endl;
        return false;
    }
    m_buf = navBuf.data();

    return true;
}

bool DumpTreeAlg::execute()
{
    JM::EvtNavigator* nav = m_buf->curEvt(); 
    if ( ! nav ) {
        LogError << "Can not retrieve the current navigator" << std::endl;
        return false;
    }

    ///////////////////////////////////////////
    auto elecHeader = dynamic_cast<JM::ElecHeader*>(nav->getHeader("/Event/Elec"));
    if ( ! elecHeader ) {
        LogError << "Failed to get ElecHeader" << std::endl;
        return false;
    }
    if ( ! elecHeader->event() ) {
        LogDebug << "ElecEvent is not valid, pass it" << std::endl;
        return true;
    }
    auto calibHeader = dynamic_cast<JM::CalibHeader*>(nav->getHeader("/Event/Calib"));
    if ( ! calibHeader ) {
        LogError << "Failed to get CalibHeader" << std::endl;
        return false;
    }

    ///////////////////////////////////////////
    auto& crate = elecHeader->event()->elecFeeCrate();
    auto calibEvt = calibHeader->event();
    //std::cout << "Calib" << std::endl;
    //for ( auto cc : calibEvt->calibPMTCol() ) {
    //    std::cout << " " << cc->pmtId() << std::endl;
    //}

    WFPmtIDs.clear();
    Waveforms.clear();
    TQPmtIDs.clear();
    Time.clear();
    Charge.clear();

    EventNo = elecHeader->EventID();
    //const TimeStamp&
    const auto& time = crate.TriggerTime();
    Second = time.GetSec();
    NanoSec = time.GetNanoSec();

    const std::map<int,JM::ElecFeeChannel>& channels = crate.channelData();
    WFPmtIDs.reserve(channels.size());
    Waveforms.reserve(channels.size());

    std::cout << "xxx nch: " << channels.size() << std::endl;
    for ( const auto& channel : channels ) {
        unsigned int pmtId = channel.first;
        unsigned int detId = CdID::id(pmtId, 0);
        //waveform
        WFPmtIDs.push_back(pmtId);
        Waveforms.push_back(std::vector<Short_t>());
        auto& src = channel.second.adc();
        auto& dest = Waveforms.back();
        dest.reserve(src.size());
        std::cout << "yyy ChID: " << pmtId << std::endl;
        for ( auto adc : src ) {
            dest.push_back(adc);
            std::cout << "zzz wf: " << adc << std::endl;
        }
        //T/Q
        auto calibPmt = calibEvt->getCalibPmtChannel(detId);
        if ( ! calibPmt ) {
            continue;
        }
        auto& charge = calibPmt->charge();
        auto& time = calibPmt->time();
        for ( unsigned int i = 0; i < calibPmt->size(); ++i ) {
            TQPmtIDs.push_back(pmtId);
            Time.push_back(time[i]);
            Charge.push_back(charge[i]);
        }
    }

    ///////////////////////////////////////////
    m_tree->Fill();

    return true;
}

bool DumpTreeAlg::finalize()
{
    return true;
}
