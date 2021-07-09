#!/bin/bash

export Middleware=`readlink -f ${BASH_SOURCE[0]} | xargs dirname`
export JUNOTOP=`dirname ${Middleware}`
MyWorkArea=`dirname ${Middleware}`
source ${JUNOTOP}/oec-adaptor/setup.sh
source ${JUNOTOP}/external/sniper/share/sniper/setup.sh
source ${JUNOTOP}/CMT/v1r26/mgr/setup.sh

export SNIPER_INIT_FILE=${OFFLINE_DIR}/config/.init.json
export CMTPROJECTPATH=${JUNOTOP}:${CMTPROJECTPATH}

export JUNO_EXTLIB_Boost_HOME=${JUNOTOP}/external/installed
export JUNO_EXTLIB_ROOT_HOME=${JUNOTOP}/external/installed/root6-24
export BOOST_PYTHON_SUFFIX="38"


export JUNO_EXTLIB_CLHEP_HOME=${JUNOTOP}/external/offline
export JUNO_EXTLIB_frontier_HOME=${JUNOTOP}/external/offline
export PATH=${JUNO_EXTLIB_CLHEP_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${JUNO_EXTLIB_CLHEP_HOME}/lib:${LD_LIBRARY_PATH}
export CPATH=${JUNO_EXTLIB_CLHEP_HOME}/include:${CPATH}

export JUNO_OECLIB_OECAdaptor_HOME=${JUNOTOP}/oec-adaptor

pushd $JUNOTOP >& /dev/null
    if [ -f "ExternalInterface/EIRelease/cmt/setup.sh" ]; then
        pushd ExternalInterface/EIRelease/cmt/ >& /dev/null
        source setup.sh
        popd >& /dev/null
    fi
    if [ -f "cmt4sniper/SniperRelease/cmt/setup.sh" ]; then
        pushd cmt4sniper/SniperRelease/cmt/ >& /dev/null
        source setup.sh
        popd >& /dev/null
    fi
popd >& /dev/null

if [ -d "${MyWorkArea}/oec-adaptor" ]; then
    export JUNO_OECLIB_OECAdaptor_HOME=${MyWorkArea}/oec-adaptor
fi
if [ "${MyWorkArea}" != "${JUNOTOP}" ]; then
    export CMTPROJECTPATH=${MyWorkArea}:${CMTPROJECTPATH}
fi

pushd ${MyWorkArea}
    if [ -f "offline/Reconstruction/Deconvolution/cmt/setup.sh" ]; then
        pushd offline/Reconstruction/Deconvolution/cmt >& /dev/null
        source setup.sh
        popd >& /dev/null
    fi
    if [ -f "oec-middleware/CppSniper4LOEC/cmt/setup.sh" ]; then
        pushd oec-middleware/CppSniper4LOEC/cmt >& /dev/null
        source setup.sh
        popd >& /dev/null
    fi
popd
