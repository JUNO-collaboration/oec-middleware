#include "LOECNavBuf.h"
#include "LOECInputSvc.h"
//#include "OEC_com/oec_com/OEC_define.h"
#include "Event/ElecHeader.h"
#include "Event/CalibHeader.h"
#include "EvtNavigator/EvtNavigator.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(LOECInputSvc);

LOECInputSvc::LOECInputSvc(const std::string& name)
    : SvcBase(name)
{
    declProp("InputType", m_type);
}

LOECInputSvc::~LOECInputSvc()
{
}

bool LOECInputSvc::initialize()
{
    if ( m_type == "Waveform" ) {
        this->m_get = &LOECInputSvc::getWaveform;
    }
    else if ( m_type == "QT" ) {
        this->m_get = &LOECInputSvc::getQT;
    }
    else {
        return false;
    }

    SniperDataPtr<LOECNavBuf> buf(getParent(), "/Event");
    if ( buf.invalid() ) {
        LogFatal << "Cannot find LOECNavBuf" << std::endl;
        return false;
    }

    m_buf = buf.data();

    LogInfo << "initialized successfully" << std::endl;

    return true;
}

bool LOECInputSvc::finalize()
{
    return true;
}

bool LOECInputSvc::getWaveform(oec::simpleBuffer& wfEvt)
{
    uint32_t* addr = (uint32_t*)wfEvt.ptr;

    JM::EvtNavigator* nav = new JM::EvtNavigator();
    m_buf->set(nav);

    JM::ElecHeader* header = new JM::ElecHeader();
    nav->addHeader(header);

    JM::ElecEvent* event = new JM::ElecEvent();
    header->setEvent(event);

    auto& chMap = const_cast<std::map<int,JM::ElecFeeChannel>&>(event->elecFeeCrate().channelData());

    //unsigned int size = wfEvt.buffer_size;
    //std::cout << "size: " << size << std::endl;
    //std::cout << std::hex;
    //for ( int i = 0; i < size/4; ++i ) {
    //    if ( i%8 == 0 ) {
    //        std::cout << std::endl;
    //    }
    //    std::cout << " 0x" << addr[i];
    //}
    //std::cout << std::dec << std::endl;

    //if ( addr[0] != 0xee1234ee || addr[6] != STREAM_TAG_WAVE ) {
    //    LogFatal << "Wrong event marker: " << *addr << std::endl;
    //    return false;
    //}

    //convert DAQ Buffer to ElecEvent
    m_buf->l1id = addr[4];
    int headerSize = addr[1]/4;  //in words
    int totalSize = addr[3]/4;   //in words

    nav->setTimeStamp(TTimeStamp(addr[8], addr[7]));
    const_cast<JM::ElecFeeCrate&>(event->elecFeeCrate()).setTriggerTime(TimeStamp(addr[8], addr[7]));
    header->setEventID(addr[5]);

    LogInfo << " L1ID: " << m_buf->l1id
        << " EventID: " << header->EventID()
        << " time: " << nav->TimeStamp() << std::endl;

    static const int wfSize = 4 + 945;  //(header + data) in words
    static const int wfSizeReal = 1250; //in 16 bits short integers
    int idx = headerSize;
    while ( idx < totalSize ) {
        //LogDebug << idx << " " << totalSize << std::endl;
        if ( idx + wfSize > totalSize ) {
            LogFatal << "Wrong Waveform size" << std::endl;
            return false;
        }
        uint16_t* pwf = (uint16_t*)(addr+idx);
        int pmtId = *pwf;
        pwf += 8;
        auto& adc = chMap[pmtId].adc();
        adc.clear();
        adc.reserve(wfSizeReal);
        for ( int i = 0; i < wfSizeReal; ++i ) {
            uint16_t value = pwf[i];
            adc.push_back(value);
        }
        idx += wfSize;
        //std::cout << "PMTID: " << pmtId << "  idx: " << idx << "  size: "<< adc.size() << std::endl;
    }

    return true;
}

bool LOECInputSvc::getQT(oec::simpleBuffer& qtEvt)
{
    uint32_t* addr = (uint32_t*)qtEvt.ptr;

    JM::EvtNavigator* nav = new JM::EvtNavigator();
    m_buf->set(nav);

    JM::CalibHeader* header = new JM::CalibHeader();
    nav->addHeader(header);

    JM::CalibEvent* event = new JM::CalibEvent();
    header->setEvent(event);

    //unsigned int size = qtEvt.buffer_size;
    //std::cout << "size: " << size << std::endl;
    //std::cout << std::hex;
    //for ( int i = 0; i < size/4; ++i ) {
    //    if ( i%8 == 0 ) {
    //        std::cout << std::endl;
    //    }
    //    std::cout << " 0x" << addr[i];
    //}
    //std::cout << std::dec << std::endl;

    //if ( addr[0] != 0xee1234ee || addr[6] != STREAM_TAG_WAVE_TQ ) {
    //    LogFatal << "Wrong event marker: " << *addr << std::endl;
    //    return false;
    //}

    //convert DAQ Buffer to ElecEvent
    m_buf->l1id = addr[4];
    int headerSize = addr[1]/4;  //in words
    int totalSize = addr[3]/4;   //in words

    nav->setTimeStamp(TTimeStamp(addr[8], addr[7]));
    header->setEventID(addr[5]);

    std::list<JM::CalibPMTChannel*> _col;
    JM::CalibPMTChannel* _ch = nullptr;
    std::vector<double> _charge;
    std::vector<double> _time;
    int lastPmtId = -1;

    static const int qtSize = 3;  //in words
    int idx = headerSize;
    while ( idx < totalSize ) {
        if ( idx + qtSize > totalSize ) {
            LogFatal << "Wrong QT size" << std::endl;
            return false;
        }
        uint32_t* pqt = (uint32_t*)(addr+idx);
        int pmtId = *pqt;
        float* pvalue = (float*)(pqt+1);
        float charge = pvalue[0];
        float time = pvalue[1];
        if ( pmtId != lastPmtId ) {
            if ( lastPmtId != -1 ) {
                _ch = new JM::CalibPMTChannel(lastPmtId);
                _ch->setCharge(_charge);
                _ch->setTime(_time);
                _col.push_back(_ch);
                _charge.clear();
                _time.clear();
            }
            lastPmtId = pmtId;
        }
        _charge.push_back(charge);
        _time.push_back(time);
        idx += qtSize;
        if ( idx == totalSize ) {
            _ch = new JM::CalibPMTChannel(pmtId);
            _ch->setCharge(_charge);
            _ch->setTime(_time);
            _col.push_back(_ch);
        }
    }

    event->setCalibPMTCol(_col);

    return true;
}
