#include "CppSniper/CppSniper4HOEC.h"
#include "HOECInputSvc.h"
#include "HOECOutputSvc.h"
#include "FragmentRingArray.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/Sniper.h"

CppSniper4HOEC::CppSniper4HOEC(const std::string& PyModule)
{
    auto fcfg = std::string{getenv("OFFLINE_DIR")} + "/config/HOECConfig.json";

    auto jtask = Sniper::eval(fcfg.c_str());

    m_task = dynamic_cast<Task*>(jtask);

    m_task->Snoopy().config();
    m_task->Snoopy().initialize();
    LogInfo<<"HOEC task is initialized"<<std::endl;

    m_input = dynamic_cast<HOECInputSvc*>(m_task->find("InputSvc"));
    m_output = dynamic_cast<HOECOutputSvc*>(m_task->find("OutputSvc"));
}

CppSniper4HOEC::~CppSniper4HOEC()
{
    m_task->Snoopy().finalize();
    delete m_task;
}


oec::OECRecEvt* CppSniper4HOEC::process(oec::OECRecEvt* vertex_ofone_ev)
{   //这一层不考虑时间片，每调用一次就会将一个事例放入task，没有处理结果时返回nullptr
    LogInfo<<"Hello this is es process(CppSniper4HOEC)."<<std::endl;
    oec::OECRecEvt* result = nullptr;
    oec::OECRecEvt* recEvt = reinterpret_cast<oec::OECRecEvt*>(vertex_ofone_ev);
    assert(recEvt->marker == 0x12345678);//检查是否拿到了LOEC写入的数据
    
    //For debug:检查接收进来的事例的时间是否正确
    static uint32_t s_localSec = 0;
    static uint32_t s_localNanoSec = 0;
    if(s_localSec < recEvt->sec);
    else if(s_localSec > recEvt->sec || (s_localSec == recEvt->sec && s_localNanoSec > recEvt->nanoSec)){
        LogError<<"Fatal Error: the HOEC task get the wrong evet time "<<std::endl;
        LogError<<"The Last event time: "<<s_localSec<<" "<<s_localNanoSec<<std::endl;
        LogError<<"THe current event time: "<<recEvt->sec<<" "<<recEvt->nanoSec<<std::endl;
        assert(false);
    }
    s_localSec = recEvt->sec;
    s_localNanoSec = recEvt->nanoSec;

    m_recEvts.push_back(recEvt);//用于记录滞留于 hoec task的events
    m_input->get(recEvt);
    while(m_input->next()){
        m_task->Snoopy().run_once();
        uint32_t _l1id = 0, _tag = 0;
        m_output->put(_l1id, _tag);
        m_recEvts.front()->tag = _tag;
        result = &(*m_recEvts.front());//将结果传出去
        m_recEvts.pop_front();
    } 
    return result;



    //旧的HOEC接口逻辑
    //oec::OECVertex vtxs((uint8_t*)(input->ptr));
    ////std::vector<oec::OECRecEvt>& qtVtxs = read_vertex.getTQVertex();
    //std::vector<oec::OECRecEvt*>& wfVtxs = vtxs.getWaveVertex();
//
    //m_input->m_l1id = vtxs.l1id();
//
    //auto& inputData = m_dataMap[m_input->m_l1id];
    //inputData.count = wfVtxs.size();
    //inputData.buf = *input;
//
    //for ( auto wfVtx : wfVtxs ) {
    //    inputData.evts.push_back(wfVtx);
    //    m_input->get(wfVtx);
//
    //    while ( m_input->next() ) {
    //        m_task->Snoopy().run_once();
    //        uint32_t _l1id = 0, _tag = 0;
    //        m_output->put(_l1id, _tag);
//
    //        auto _it = m_dataMap.find(_l1id);
    //        auto& _data = (*_it).second;
    //        auto& _evts = _data.evts;
    //        _evts.front()->tag = _tag;
    //        _evts.pop_front();
    //        if ( --_data.count == 0 ) {
    //            //std::cout << __FILE__ << __LINE__ << " completed l1id: " << _l1id
    //            //    << " event number: " << oec::OECVertex((uint8_t*)(_data.buf.ptr)).getWaveVertex().size()
    //            //    << std::endl;
    //            output->addsimbuf(_l1id, _data.buf);
    //            m_dataMap.erase(_it);
    //        }
    //    }
    //}
}
