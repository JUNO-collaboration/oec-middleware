#include "LOECNavBuf.h"
#include "LOECOutputSvc.h"
#include "Event/CalibHeader.h"
#include "Event/OECHeader.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SvcFactory.h"
#include "OEC_com/oec_com/OEC_define.h"

DECLARE_SERVICE(LOECOutputSvc);

LOECOutputSvc::LOECOutputSvc(const std::string& name)
    : SvcBase(name)
{
    declProp("OutputType", m_type);
}

LOECOutputSvc::~LOECOutputSvc()
{
}

bool LOECOutputSvc::initialize()
{
    if ( m_type == "QT" ) {
        this->m_put = &LOECOutputSvc::putQT;
    }
    else if ( m_type == "Vertex" ) {
        this->m_put = &LOECOutputSvc::putVertex;
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

bool LOECOutputSvc::finalize()
{
    return true;
}

bool LOECOutputSvc::put(oec::simpleBuffer& output)
{
    return (this->*m_put)(output);
}

bool LOECOutputSvc::putQT(oec::simpleBuffer& qtEvt)
{
    //DAQ Buffer
    uint32_t* _cache = (uint32_t*)qtEvt.ptr;

    //prepare result
    JM::EvtNavigator* nav = m_buf->curEvt();
    auto calibHeader = dynamic_cast<JM::CalibHeader*>(nav->getHeader("/Event/Calib"));
    if ( ! calibHeader ) {
        LogFatal << "Failed to get CalibHeader" << std::endl;
        return false;
    }
    auto& calibCol = calibHeader->event()->calibPMTCol();

    //set result to DAQ Buffer
    static const int headerSize = 9;
    int size = headerSize;
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

    qtEvt.buffer_size = sizeInBytes;

    //...
    m_buf->clear();

    return true;
}

bool LOECOutputSvc::putVertex(oec::simpleBuffer& vtxEvt)
{
    //DAQ Buffer
    uint32_t* _cache = (uint32_t*)vtxEvt.ptr;

    //prepare result
    JM::EvtNavigator* nav = m_buf->curEvt();
    auto oecHeader = dynamic_cast<JM::OECHeader*>(nav->getHeader("/Event/OEC"));
    if ( ! oecHeader ) {
        LogFatal << "Failed to get OECHeader" << std::endl;
        return false;
    }
    auto oecEvent = (JM::OECEvent*)oecHeader->event("JM::OECEvent");

    //set result to DAQ Buffer
    static const int headerSize = 5;
    oec::OECRecEvt* _evt = (oec::OECRecEvt*)(_cache+headerSize);
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

    //set the header
    int sizeInBytes = headerSize*4 + sizeof(oec::OECRecEvt);
    _cache[0] = 0x12345678;  //marker
    _cache[1] = headerSize*4;  //HeaderSize
    _cache[2] = 0;  //qt vertex size in bytes
    _cache[3] = sizeof(oec::OECRecEvt);  //waveform vertex size in bytes
    _cache[4] = m_buf->l1id;  //l1id

    vtxEvt.buffer_size = sizeInBytes;

    LogInfo << " L1ID: " << _cache[4]
        << " EventID: " << _evt->evtId
        << " TAG: 0x" << std::hex << _evt->tag << std::dec
        << " time: " << time
        << " energy: " << _evt->energy
        << " vertex: [" << _evt->x
        << ", " << _evt->y
        << ", " << _evt->z
        << "]" << std::endl;

    //...
    m_buf->clear();

    return true;
}