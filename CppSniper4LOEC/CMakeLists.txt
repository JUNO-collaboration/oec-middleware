PKG(CppSniper4LOEC
    DEPENDS
        offline::EvtNavigator
        offline::ElecEvent
        offline::CalibEvent
        offline::OECEvent
        Boost::thread)

target_link_libraries(CppSniper4LOEC oec_com)