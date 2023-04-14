cmake_minimum_required(VERSION 3.15)

project(oec-middleware)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

set(Boost_PYTHON_VERSION "3.8")
find_package(junosw)
include(PKG)

find_package(Boost REQUIRED COMPONENTS python38 thread system filesystem)
if (Boost_FOUND)
  message("Found Boost: ${Boost_LIBRARIES}")
endif()
include_directories($ENV{JUNO_EXTLIB_Boost_HOME})
link_directories($ENV{JUNO_EXTLIB_Boost_HOME}/lib)


###oec-adaptor
# include_directories($ENV{JUNO_OECLIB_OECAdaptor_HOME}/user/processor)
include_directories($ENV{JUNO_OECLIB_OECAdaptor_HOME}/df/pack_plus)
include_directories($ENV{JUNO_OECLIB_OECAdaptor_HOME}/df/oec_com)
link_directories($ENV{JUNO_OECLIB_OECAdaptor_HOME}/installed/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-needed -Wall -ggdb -Og -g")

add_subdirectory(CppSniper4LOEC)
add_subdirectory(CppSniper4HOEC)
add_subdirectory(MiddlewareConfigSvc)
add_subdirectory(OecEvtConverter)
add_subdirectory(OECTestAlg)
