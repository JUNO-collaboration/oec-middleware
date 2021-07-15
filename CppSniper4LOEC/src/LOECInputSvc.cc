#include "LOECNavBuf.h"
#include "LOECInputSvc.h"
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
    //std::cout<<"Start get waveform "<<std::endl;
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    m_buf->set(nav);

    JM::ElecHeader* header = new JM::ElecHeader();
    nav->addHeader(header);

    JM::ElecEvent* event = new JM::ElecEvent();
    header->setEvent(event);

    //event->setElecFeeCrate(JM::ElecFeeCrate());
    auto& chMap = const_cast<std::map<int,JM::ElecFeeChannel>&>(event->elecFeeCrate().channelData());

    //convert DAQ Buffer to ElecEvent
    uint32_t* addr = (uint32_t*)wfEvt.ptr;

    //unsigned int size = wfEvt->size();
    //std::cout << "size: " << size << std::endl;
    //std::cout << std::hex;
    //for ( int i = 0; i < size/4; ++i ) {
    //    if ( i%8 == 0 ) {
    //        std::cout << std::endl;
    //    }
    //    std::cout << " 0x" << addr[i];
    //}
    //std::cout << std::dec << std::endl;

    if ( addr[0] != 0xee1234ee || addr[6] != 0x10002/*STREAM_TAG_WAVE*/ ) {
        LogFatal << "Wrong event marker: " << *addr << std::endl;
        return false;
    }

    m_buf->l1id = addr[4];
    int headerSize = addr[1]/4;  //in words
    int totalSize = addr[3]/4;   //in words

    //FIXME: this is a patch, because the quantity of ns from TF exceeds 1s, and exceed the limit signed_int which is the formal parameter of TimeStamp 
    addr[8] += addr[7]/1000000000;
    addr[7] %= 1000000000;

    nav->setTimeStamp(TTimeStamp((time_t)addr[8], (Int_t)addr[7]));
    //used to debug, print timeStamp
    //LogInfo<<"**********************TimeStamp"<<nav->TimeStamp()<<std::endl;
    const_cast<JM::ElecFeeCrate&>(event->elecFeeCrate()).setTriggerTime(TimeStamp(addr[8], addr[7]));
    header->setEventID(addr[5]);

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
    }
    //std::cout<<"get waveform end"<<std::endl;
    return true;
}


