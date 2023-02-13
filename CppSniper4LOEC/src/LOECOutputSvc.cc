#include "LOECNavBuf.h"
#include "LOECOutputSvc.h"
#include "Event/CdLpmtCalibHeader.h"
#include "Event/OecHeader.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include <iostream>
#include "oec_com/OEC_define.h"
#include "SniperKernel/SniperPtr.h"
#include "RootIOSvc/RootOutputSvc.h"

DECLARE_SERVICE(LOECOutputSvc);

LOECOutputSvc::LOECOutputSvc(const std::string& name)
    : SvcBase(name),
      m_totalInPack(0),
      m_nInPack(0)
{
    declProp("OutputType", m_type);
    m_cache = new char[1024*1024*4]; //FIXME: should not use a fixed size
    m_converter = new OecEvtConverter((uint8_t*)m_cache);
}

LOECOutputSvc::~LOECOutputSvc()
{
    delete [] (char*)m_cache;
}

bool LOECOutputSvc::initialize()
{
    ////用于和离线结果进行对比，将转换后的数据存下来
    //SniperPtr<RootOutputSvc> oSvc(*m_par, "ValiOutputSvc");
    //if ( oSvc.invalid() ) {
    //    LogFatal << "cann't find OutputSvc for "
    //             << m_par->scope() << m_par->objName() << std::endl;
    //    throw SniperException("OutputSvc is invalid");
    //}
    //m_oSvc = oSvc.data();

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
    int sizeInBytes = m_converter->writeOecEvt(nav, onlineEvt.l1id());

    std::pair<uint8_t*, uint8_t*> recResultInterval = onlineEvt.indexOf(junoread::Event::EventIndex::REC_RESULT);
    
    uint32_t recTag = *reinterpret_cast<uint32_t*>(recResultInterval.first);
    assert((recResultInterval.second - recResultInterval.first) == 12);
    assert((recTag >> 16) == static_cast<uint32_t>(junoread::data_type::REC_RESULT));
    assert((recTag & 0x0000ffff) == 0x0000ffff);
    LogInfo<<"sizeInBytes  of vertex "<<sizeInBytes<<std::endl;
    assert(sizeInBytes < 6000000);//daq提供的内存上限是6M

    void* recResult = reinterpret_cast<void*>(*reinterpret_cast<uint64_t*>(recResultInterval.first + 4));
    memcpy(recResult, m_cache, sizeInBytes);

    ////为了和离线作比较将数据写出
    //bool okay = m_oSvc->write(nav);
    //if (!okay) {
    //  // If writing is failed, end run immediately
    //  // FIXME Not an elegant way
    //  LogFatal << "Failed to write event data!!!" << std::endl;
    //  assert(okay);
    //}

    return true;
}

void LOECOutputSvc::clear(){
    m_buf->clear();
}
