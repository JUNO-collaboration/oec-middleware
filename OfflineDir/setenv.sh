#!/bin/bash

export OFFLINE_DIR=`readlink -f ${BASH_SOURCE[0]} | xargs dirname`
echo "[INFO] use offline libraries: \"${OFFLINE_DIR}\""
export LD_LIBRARY_PATH=${OFFLINE_DIR}/lib:$LD_LIBRARY_PATH
export PYTHONPATH=${OFFLINE_DIR}/config:${OFFLINE_DIR}/python:$PYTHONPATH
export SNIPER_INIT_FILE=${OFFLINE_DIR}/config/.init.json
export WORKTOP=$OFFLINE_DIR

if [[ -z "${JUNOTOP}" ]];
then 
    export JUNOTOP=$OFFLINE_DIR
fi

if [[ -z "${JUNO_GEOMETRY_PATH}" ]];###########Fix me::  RecGeomSvc从这里取配置文件
then 
    export JUNO_GEOMETRY_PATH=/home/yangyx/OEC_project/junosw/Detector/Geometry
fi
