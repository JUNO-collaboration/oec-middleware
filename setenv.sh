#!/bin/bash

export JUNOTOP=/home/bes/zoujh/besfs5/JunoOEC
MyWorkArea=/home/bes/zoujh/besfs5/JunoOEC

source ${JUNOTOP}/oec-adaptor/setup.sh
source ${JUNOTOP}/CMT/v1r26/mgr/setup.sh

export CMTPROJECTPATH=${JUNOTOP}:${CMTPROJECTPATH}

export JUNO_EXTLIB_Boost_HOME=${JUNOTOP}/external/installed
export JUNO_EXTLIB_ROOT_HOME=${JUNOTOP}/external/installed/root6
export BOOST_PYTHON_SUFFIX=""


export JUNO_EXTLIB_CLHEP_HOME=${JUNOTOP}/external/offline
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
    if [ -f "sniper/SniperRelease/cmt/setup.sh" ]; then
        pushd sniper/SniperRelease/cmt/ >& /dev/null
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
