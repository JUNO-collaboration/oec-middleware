#include "LOECNavBuf.h"
#include "LOECOutputSvc.h"
#include "Event/CalibHeader.h"
#include "Event/OECHeader.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"
#include "OEC_com/oec_com/OEC_define.h"
#include "SniperKernel/SniperLog.h"
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

bool LOECOutputSvc::putQT(oec::simpleBuffer& qtEvt)
{
    JM::EvtNavigator* nav = m_buf->curEvt();
    auto calibHeader = dynamic_cast<JM::CalibHeader*>(nav->getHeader("/Event/Calib"));
    if ( ! calibHeader ) {
        LogFatal << "Failed to get CalibHeader" << std::endl;
        return false;
    }
    auto& calibCol = calibHeader->event()->calibPMTCol();
    //int size = calibCol.size();
    //std::cout << "zzz " << size << std::endl;

    //set result to DAQ Buffer
    static const int headerSize = 9;
    int size = headerSize;
    uint32_t* _cache = (uint32_t*)m_cache;
    for ( auto calib : calibCol ) {
        int pmtId = calib->pmtId();
        auto& charge = calib->charge();
        auto& time = calib->time();
        for ( unsigned int i = 0; i < calib->size(); ++i ) {
            _cache[size] = pmtId;
            float _charge = charge[i];
            float _time = time[i];
            _cache[size+1] = *((uint32_t*)(&_charge));
            _cache[size+2] = *((uint32_t*)(&_time));
            size += 3;
        }
    }
    //set the header
    int sizeInBytes = size*4;
    _cache[0] = 0xee1234ee;  //marker
    _cache[1] = 36;  //HeaderSize
    _cache[2] = 0;  //
    _cache[3] = sizeInBytes;  //TotalSize in bytes
    _cache[4] = m_buf->l1id;  //l1id
    _cache[5] = calibHeader->EventID();  //EventID
    _cache[6] = 0x00010003;  //TAG
    const auto& time = nav->TimeStamp();
    _cache[7] = time.GetNanoSec();  //NanoSec
    _cache[8] = time.GetSec();  //Sec

    //set the DAQ Buffer
    qtEvt.buffer_size = sizeInBytes;
    
    memcpy(qtEvt.ptr, _cache, sizeInBytes);

    

    return true;
}

bool LOECOutputSvc::putVertex(oec::simpleBuffer& vtxEvt)
{
    //std::cout << "Try to get Navigator" << std::endl;
    JM::EvtNavigator* nav = m_buf->curEvt();
    //std::cout << "Try to get Header" << std::endl;
    auto oecHeader = dynamic_cast<JM::OECHeader*>(nav->getHeader("/Event/OEC"));
    if ( ! oecHeader ) {
        LogFatal << "Failed to get OECHeader" << std::endl;
        return false;
    }
    auto oecEvent = (JM::OECEvent*)oecHeader->event("JM::OECEvent");

    //set result to DAQ Buffer
    static const int headerSize = 5;
    int size = headerSize;
    uint32_t* _cache = (uint32_t*)m_cache;
    oec::OECRecEvt* _evt = (oec::OECRecEvt*)(_cache+size);
    _evt->evtId = oecHeader->EventID();
    const auto& time = nav->TimeStamp();
    _evt->sec = time.GetSec();
    _evt->nanoSec = time.GetNanoSec();
    _evt->nCluster = 0; //not used yet
    _evt->tag = oecEvent->getTag();
    _evt->energy = oecEvent->getEnergy();
    _evt->x = oecEvent->getVertexX();
    _evt->y = oecEvent->getVertexY();
    _evt->z = oecEvent->getVertexZ();

    int sizeInBytes = size*4 +sizeof(oec::OECRecEvt);
    _cache[0] = 0x12345678;  //marker
    _cache[1] = headerSize*4;  //HeaderSize
    _cache[2] = 0;  //qt vertex size in bytes
    _cache[3] = sizeof(oec::OECRecEvt);  //waveform vertex size in bytes
    _cache[4] = m_buf->l1id;  //l1id

    //set the DAQ Buffer
    vtxEvt.buffer_size = sizeInBytes;
    
    memcpy(vtxEvt.ptr, _cache, sizeInBytes);

    return true;
}

void LOECOutputSvc::clear(){
    m_buf->clear();
}