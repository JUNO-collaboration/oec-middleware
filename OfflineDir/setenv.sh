#!/bin/bash

export OFFLINE_DIR=`readlink -f ${BASH_SOURCE[0]} | xargs dirname`
#echo "[INFO] use offline libraries: \"${OFFLINE_DIR}\""
export LD_LIBRARY_PATH=${OFFLINE_DIR}/lib:$LD_LIBRARY_PATH
export PYTHONPATH=${OFFLINE_DIR}/config:${OFFLINE_DIR}/python:$PYTHONPATH
export SNIPER_INIT_FILE=${OFFLINE_DIR}/config/.init.json
