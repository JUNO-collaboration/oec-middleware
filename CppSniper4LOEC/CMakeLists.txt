PKG(CppSniper4LOEC
    DEPENDS
        Boost::system
        #Boost::thread
        #Boost::python38
        junosw::EvtNavigator
        junosw::ElecEvent
        junosw::CalibEvent
        junosw::OecEvent
        junosw::Geometry
        junosw::CondDB
        junosw::BufferMemMgr
        MiddlewareConfigSvc
        #junosw::OECTagSvc
        #junosw::EvtConfig
        )

target_link_libraries(CppSniper4LOEC oec_com)
target_link_libraries(CppSniper4LOEC juno_pack)
target_link_libraries(CppSniper4LOEC pthread)
#target_link_libraries(CppSniper4LOEC xerces-c)
