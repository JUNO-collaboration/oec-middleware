#include "CppSniper/CppSniper4HOEC.h"
#include "HOECInputSvc.h"
#include "HOECOutputSvc.h"
#include "parse.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/Sniper.h"

CppSniper4HOEC::CppSniper4HOEC(const std::string& PyModule)
{
    auto fcfg = std::string{getenv("OFFLINE_DIR")} + "/config/HOECConfig.json";
    auto jtask = Sniper::eval(fcfg.c_str());
    m_task = dynamic_cast<Task*>(jtask);

    m_task->Snoopy().config();
    m_task->Snoopy().initialize();

    m_input = dynamic_cast<HOECInputSvc*>(m_task->find("InputSvc"));
    m_output = dynamic_cast<HOECOutputSvc*>(m_task->find("OutputSvc"));
}

CppSniper4HOEC::~CppSniper4HOEC()
{
    m_task->Snoopy().finalize();
    delete m_task;
}

void CppSniper4HOEC::process(oec::simpleBuffer* input, oec::EventDepository* output)
{
    //unsigned int size = input->buffer_size;
    //uint32_t* addr = (uint32_t*)input->ptr;
    //std::cout << "size: " << size << std::endl;
    //std::cout << std::hex;
    //for ( int i = 0; i < size/4; ++i ) {
    //    if ( i%8 == 0 ) {
    //        std::cout << std::endl;
    //    }
    //    std::cout << " 0x" << addr[i];
    //}
    //std::cout << std::dec << std::endl;

    oec::OECVertex vtxs((uint8_t*)(input->ptr));
    //std::vector<oec::OECRecEvt>& qtVtxs = read_vertex.getTQVertex();
    std::vector<oec::OECRecEvt*>& wfVtxs = vtxs.getWaveVertex();

    m_input->m_l1id = vtxs.l1id();

    auto& inputData = m_dataMap[m_input->m_l1id];
    inputData.count = wfVtxs.size();
    inputData.buf = *input;

    for ( auto wfVtx : wfVtxs ) {
        inputData.evts.push_back(wfVtx);
        m_input->get(wfVtx);

        while ( m_input->next() ) {
            m_task->Snoopy().run_once();
            uint32_t _l1id = 0, _tag = 0;
            m_output->put(_l1id, _tag);

            auto _it = m_dataMap.find(_l1id);
            auto& _data = (*_it).second;
            auto& _evts = _data.evts;
            _evts.front()->tag = _tag;
            _evts.pop_front();
            if ( --_data.count == 0 ) {
                //std::cout << __FILE__ << __LINE__ << " completed l1id: " << _l1id
                //    << " event number: " << oec::OECVertex((uint8_t*)(_data.buf.ptr)).getWaveVertex().size()
                //    << std::endl;
                output->addsimbuf(_l1id, _data.buf);
                m_dataMap.erase(_it);
            }
        }
    }
}
