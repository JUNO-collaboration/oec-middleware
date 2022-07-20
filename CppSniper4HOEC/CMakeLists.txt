PKG(CppSniper4HOEC
    DEPENDS
        Boost::system
        #Boost::python38
        offline::EvtNavigator
        offline::OecEvent
        )


target_link_libraries(CppSniper4LOEC oec_com)
target_link_libraries(CppSniper4LOEC juno_pack)
target_link_libraries(CppSniper4LOEC pthread)