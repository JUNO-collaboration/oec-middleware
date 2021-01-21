#include "OECTestAlg.h"
#include "Event/ElecHeader.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"

DECLARE_ALGORITHM(OECTestAlg);

OECTestAlg::OECTestAlg(const std::string& name)
    : AlgBase(name)
{
    m_count = 0;
}

bool OECTestAlg::initialize()
{
    SniperDataPtr<JM::NavBuffer> buf(getParent(), "/Event");
    if ( buf.invalid() ) {
        LogFatal << "Cannot find LOECNavBuf" << std::endl;
        return false;
    }

    m_buf = buf.data();

    return true;
}

bool OECTestAlg::execute()
{
    ++m_count;

    auto nav = m_buf->curEvt();
    if ( ! nav ) {
        LogError << "Can not retrieve the current navigator" << std::endl;
        return false;
    }

    //auto elecHeader = dynamic_cast<JM::ElecHeader*>(nav->getHeader("/Event/Elec"));
    //if ( elecHeader && elecHeader->event() ) {
    //    auto& crate = elecHeader->event()->elecFeeCrate();
    //    const std::map<int,JM::ElecFeeChannel>& channels = crate.channelData();
    //    std::cout << "xxx nch: " << channels.size() << std::endl;
    //    for ( const auto& channel : channels ) {
    //        std::cout << "yyy ChID: " << channel.first << std::endl;
    //        auto& adcs = channel.second.adc();
    //        for ( auto adc : adcs ) {
    //            std::cout << "zzz wf: " << adc << std::endl;
    //        }
    //    }
    //}

    LogDebug << "running event " << m_count << std::endl;

    return true;
}

bool OECTestAlg::finalize()
{
    return true;
}
