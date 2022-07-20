PKG(CppSniper4LOEC
    DEPENDS
        Boost::system
        #Boost::thread
        #Boost::python38
        offline::EvtNavigator
        offline::ElecEvent
        offline::CalibEvent
        offline::OecEvent
        offline::Geometry
        offline::CondDB
        offline::BufferMemMgr
        #offline::OECTagSvc
        #offline::EvtConfig
        )

target_link_libraries(CppSniper4LOEC oec_com)
target_link_libraries(CppSniper4LOEC juno_pack)
target_link_libraries(CppSniper4LOEC pthread)
#target_link_libraries(CppSniper4LOEC xerces-c)
