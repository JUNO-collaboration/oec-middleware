#!/bin/bash

export Middleware=`readlink -f ${BASH_SOURCE[0]} | xargs dirname`
export JUNOTOP=`dirname ${Middleware}`
MyWorkArea=`dirname ${Middleware}`
source ${JUNOTOP}/oec-adaptor/setup.sh
source ${JUNOTOP}/sniper/InstallArea/share/sniper/setup.sh
source ${JUNOTOP}/offline/InstallArea/setup.sh
#source ${JUNOTOP}/CMT/v1r26/mgr/setup.sh

 
#export TBB_ROOT=${JUNOTOP}/external/oneTBB-2020.1 
export SNIPER_INIT_FILE=${OFFLINE_DIR}/config/.init.json

export JUNO_EXTLIB_Boost_HOME=${JUNOTOP}/external/installed
export JUNO_EXTLIB_ROOT_HOME=${JUNOTOP}/external/installed/root6-24_cxx17
export BOOST_PYTHON_SUFFIX="38"

export JUNO_OECLIB_OECAdaptor_HOME=${JUNOTOP}/oec-adaptor

#external of offline
export JUNO_EXTLIB_CLHEP_HOME=${JUNOTOP}/external/CLHEP
export JUNO_EXTLIB_frontier_HOME=${JUNOTOP}/external/frontier
export JUNO_EXTLIB_pacparser_HOME=${JUNOTOP}/external/pacparser
export JUNO_EXTLIB_Xercesc_HOME=${JUNOTOP}/external/xercesc

source ${JUNO_EXTLIB_CLHEP_HOME}/bashrc
source ${JUNO_EXTLIB_frontier_HOME}/bashrc
source ${JUNO_EXTLIB_pacparser_HOME}/bashrc
source ${JUNO_EXTLIB_Xercesc_HOME}/bashrc


if [ -d "${MyWorkArea}/oec-adaptor" ]; then
    export JUNO_OECLIB_OECAdaptor_HOME=${MyWorkArea}/oec-adaptor
fi
if [ "${MyWorkArea}" != "${JUNOTOP}" ]; then
    export CMTPROJECTPATH=${MyWorkArea}:${CMTPROJECTPATH}
fi


