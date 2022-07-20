#include "LOECNavBuf.h"
#include "LOECInputSvc.h"
#include "Event/CdWaveformHeader.h"
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

bool LOECInputSvc::getWaveform(junoread::Event& onlineEvt)
{   
    //std::cout<<"Start get waveform "<<std::endl;
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    m_buf->set(nav);

    auto header = new JM::CdWaveformHeader();
    nav->addHeader(header);

    auto event = new JM::CdWaveformEvt();
    header->setEvent(event);

    //event->setElecFeeCrate(JM::ElecFeeCrate());

    m_buf->l1id = onlineEvt.l1id();

    //获取事例时间
    uint64_t _evtTime = onlineEvt.evTime(); //uint64_t 存储的是一个ns数
    time_t _second = (int)(_evtTime/1000000000);//获得秒数
    Int_t _nanoSec = (int)(_evtTime%1000000000);//获得纳秒数
    nav->setTimeStamp(TTimeStamp(_second, _nanoSec)); 
    //used to debug, print timeStamp
    //LogInfo<<"**********************TimeStamp"<<nav->TimeStamp()<<std::endl;
    
    nav->setEventID(onlineEvt.evId());

    //获取存储波形数据区间的头指针和尾后指针，区段内存放格式：4B ChannelTag 和 8B的数据指针 + 4B ChannelTag......
    const std::pair<uint8_t*, uint8_t*> wfInterval = onlineEvt.indexOf(junoread::Event::EventIndex::WAVEFORM);
   


    //循环获取每个Channel的波形数据
    for(auto channelPtr = wfInterval.first; channelPtr != wfInterval.second;){
        
        uint32_t channelTag = *reinterpret_cast<uint32_t*>(channelPtr);
        assert((channelTag>>16)==static_cast<uint32_t>(junoread::data_type::WAVEFORME));//channel高16位数据类型，低16位通道号
        
        uint32_t channelId = 0x0000ffff & channelTag;

        uint16_t* dataPtr = reinterpret_cast<uint16_t*>(*reinterpret_cast<uint64_t*>(channelPtr+4));//获取存放此通道波形内存块的指针，这块内存是按照16位存放的
        assert(*dataPtr == 0x805a);
        size_t dataSize = (junoread::ELEC_DATA_SIZE.at(junoread::data_type::WAVEFORME))/2;//一个通道的波形数据长度（存了多少个uint_16），包含header和trailer
        assert(*(dataPtr + dataSize - 1) == 0x0869);//校验波形最后末尾的uint16_t

        uint16_t* wfPtr = dataPtr + 8;//跳过前面8个uint16_t数，指向波形的第一个数字
        
        // size_t wfSizeReal = dataSize - 16 - 6; ///去除8个uint16_t的header和trailer，波形的长度, 为了对其原因，波形数据最后添了6个空位
        // assert(wfSizeReal == 1250);//离线要求接受的波形长度是1250
        size_t wfSizeReal = dataSize - 16; //在线数据格式 没有最后添6个空位
        assert(wfSizeReal == 1000 || wfSizeReal - 6 == 1250);//在线要求接受的波形长度是1000

        assert(*(wfPtr + dataSize -16) == 0x55aa);

        auto _channel = new JM::ElecWaveform();
        event->addChannel(channelId, _channel);
        auto& charges = _channel->adc();
        charges.reserve(wfSizeReal);

        //填入波形
        for(int i = 0; i < wfSizeReal; i++){
            charges.push_back(wfPtr[i]);
        }
        channelPtr += 12;//channel_tag 4B + pointer 8B
    }
    return true;
}


