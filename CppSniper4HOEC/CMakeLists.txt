PKG(CppSniper4HOEC
    DEPENDS
        Boost::system
        Boost::python38
        offline::EvtNavigator
        offline::OECEvent
        )


target_link_libraries(CppSniper4LOEC oec_com)