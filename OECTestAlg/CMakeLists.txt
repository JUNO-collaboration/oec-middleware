PKG(OECTestAlg
    DEPENDS
        OecEvtConverter
        junosw::OecEvent
        )

#需要写上对外部的依赖     
target_link_libraries(CppSniper4LOEC oec_com)
target_link_libraries(CppSniper4LOEC juno_pack)