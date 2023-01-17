#include "LOECNavBuf.h"
#include "LOECOutputSvc.h"
#include "Event/CdLpmtCalibHeader.h"
#include "Event/OecHeader.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include <iostream>
#include "oec_com/OEC_define.h"
DECLARE_SERVICE(LOECOutputSvc);

LOECOutputSvc::LOECOutputSvc(const std::string& name)
    : SvcBase(name),
      m_totalInPack(0),
      m_nInPack(0)
{
    declProp("OutputType", m_type);
    m_cache = new char[1024*1024*4]; //FIXME: should not use a fixed size
}

LOECOutputSvc::~LOECOutputSvc()
{
    delete [] (char*)m_cache;
}

bool LOECOutputSvc::initialize()
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

bool LOECOutputSvc::finalize()
{
    return true;
}



bool LOECOutputSvc::putQT(junoread::Event& onlineEvt){
    JM::EvtNavigator* nav = m_buf->curEvt();
    auto calibHeader = dynamic_cast<JM::CdLpmtCalibHeader*>(nav->getHeader("/Event/CdLpmtCalib"));
    if ( ! calibHeader ) {
        LogFatal << "Failed to get CdLpmtCalibHeader" << std::endl;
        return false;
    }
    auto& calibCol = calibHeader->event()->calibPMTCol();
    
    //###################For test#######
    int _calibNum = 0;
    int _chargeExtreme = 0;
    int _chargeNum = 0;
    int _count = 0;

    //###################For test#######

    static const int headerSize = 9;
    int size = headerSize;
    uint32_t* _cache = (uint32_t*)m_cache;
    for ( auto calib : calibCol ) {
        _calibNum++;                      //DM: for test
        if(_chargeNum > _chargeExtreme)   //DM: for test
            _chargeExtreme = _chargeNum;  //DM: for test
        if(_chargeNum > 20)               //DM: for test
            _count++;                     //DM: for test  
        _chargeNum = 0;                   //DM: for test

        int pmtId = calib->pmtId();
        auto& charge = calib->charge();
        auto& time = calib->time();
        for ( unsigned int i = 0; i < calib->size(); ++i ) {
            _chargeNum++;//DM: for test
            _cache[size] = pmtId;
            float _charge = charge[i];
            float _time = time[i];
            _cache[size+1] = *((uint32_t*)(&_charge));
            _cache[size+2] = *((uint32_t*)(&_time));
            size += 3;
        }
    }
    LogInfo<<"The number of Pmt "<<_calibNum<<std::endl;
    LogInfo<<"The upper limi of the number of charge per pmt   "<<_chargeExtreme<<std::endl;
    LogInfo<<"the number of Charges exceeding 20 "<<_count<<std::endl;

    //set the header
    int sizeInBytes = size*4;
    _cache[0] = 0xee1234ee;  //marker
    _cache[1] = 36;  //HeaderSize in Bytes
    _cache[2] = 0;  //
    _cache[3] = sizeInBytes;  //TotalSize in bytes
    _cache[4] = m_buf->l1id;  //l1id
    _cache[5] = nav->EventID();  //EventID
    _cache[6] = 0x00010003;  //TAG
    const auto& time = nav->TimeStamp();
    _cache[7] = time.GetNanoSec();  //NanoSec
    _cache[8] = time.GetSec();  //Sec

    std::pair<uint8_t*, uint8_t*> tqInterval = onlineEvt.indexOf(junoread::Event::EventIndex::WAVEFORM_TQ);

    assert(tqInterval.second - tqInterval.first == 12);//前4Bytes存放channelTag（用于标识是tq），后8个Byte是指针，指向存TQ的内存（大小~1M）
    uint32_t TQtag = *reinterpret_cast<uint32_t*>(tqInterval.first);
    assert((TQtag >> 16) == static_cast<uint32_t>(junoread::data_type::WAVE_CONSTRUCTED_TQ));
    assert((TQtag & 0x0000ffff) == 0x0000fffd);
    LogInfo<<"sizeInBytes  of TQ "<<sizeInBytes<<std::endl;
    assert(sizeInBytes < 6000000);//daq提供的内存上限是1M

    void* recTQresult = reinterpret_cast<void*>(*reinterpret_cast<uint64_t*>(tqInterval.first + 4));
    memcpy(recTQresult, _cache, sizeInBytes);

    return true;
}


bool LOECOutputSvc::putVertex(junoread::Event& onlineEvt)
{
    //std::cout << "Try to get Navigator" << std::endl;
    JM::EvtNavigator* nav = m_buf->curEvt();
    //std::cout << "Try to get Header" << std::endl;
    auto oecHeader = dynamic_cast<JM::OecHeader*>(nav->getHeader("/Event/Oec"));
    if ( ! oecHeader ) {
        LogFatal << "Failed to get OecHeader" << std::endl;
        return false;
    }
    auto oecEvent = (JM::OecEvt*)oecHeader->event("JM::OecEvt");

    //set result to DAQ Buffer
    static const int headerSize = 0;//重建的顶点结果不要header
    int size = headerSize;
    uint32_t* _cache = (uint32_t*)m_cache;
    oec::OECRecEvt* _evt = (oec::OECRecEvt*)(_cache+size);
    _evt->marker = 0x12345678;
    _evt->l1id = onlineEvt.l1id();
    _evt->evtId = nav->EventID();
    const auto& time = nav->TimeStamp();
    _evt->sec = time.GetSec();
    _evt->nanoSec = time.GetNanoSec();
    _evt->nCluster = 0; //not used yet
    _evt->tag = oecEvent->getTag();
    _evt->energy = oecEvent->getEnergy();
    _evt->x = oecEvent->getVertexX();
    _evt->y = oecEvent->getVertexY();
    _evt->z = oecEvent->getVertexZ();
    //FixME: muon的顶点可能又多个，新的OECEvt中存放的是一个vector
    //_evt->muinx = oecEvent->getMuInX();
    //_evt->muiny = oecEvent->getMuInY();
    //_evt->muinz = oecEvent->getMuInZ();
    //_evt->muid  = oecEvent->getMuID();

    int sizeInBytes = size*4 +sizeof(oec::OECRecEvt);//把 header拿到结构体部
    //_cache[0] = 0x12345678;  //marker
    //_cache[1] = headerSize*4;  //HeaderSize
    //_cache[2] = 0;  //qt vertex size in bytes
    //_cache[3] = sizeof(oec::OECRecEvt);  //waveform vertex size in bytes
    //_cache[4] = m_buf->l1id;  //l1id

    std::pair<uint8_t*, uint8_t*> recResultInterval = onlineEvt.indexOf(junoread::Event::EventIndex::REC_RESULT);
    
    uint32_t recTag = *reinterpret_cast<uint32_t*>(recResultInterval.first);
    assert((recResultInterval.second - recResultInterval.first) == 12);
    assert((recTag >> 16) == static_cast<uint32_t>(junoread::data_type::REC_RESULT));
    assert((recTag & 0x0000ffff) == 0x0000ffff);
    LogInfo<<"sizeInBytes  of vertex "<<sizeInBytes<<std::endl;
    assert(sizeInBytes < 6000000);//daq提供的内存上限是6M

    void* recResult = reinterpret_cast<void*>(*reinterpret_cast<uint64_t*>(recResultInterval.first + 4));
    memcpy(recResult, _cache, sizeInBytes);

    return true;
}

void LOECOutputSvc::clear(){
    m_buf->clear();
}
