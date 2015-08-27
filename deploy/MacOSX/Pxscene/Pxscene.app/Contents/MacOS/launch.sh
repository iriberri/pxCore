#!/bin/bash
BASE_DIR=`dirname $0`
cd ${BASE_DIR}
BASE_DIR=`pwd`
PX_VERSION=`cat ${BASE_DIR}/version`
AUTO_UPDATE=${BASE_DIR}/autoupdate
UPDATE_URL=http://www.pxscene.org/dist/osx/Info.plist
PRODUCT_ID=com.comcast.pxscene
LOG_DIR=/var/tmp/pxscene/logs
LOG_FILE=${LOG_DIR}/launch.out
DEBUG_FILE=${LOG_DIR}/debug.out
NODE_DIR=${BASE_DIR}/../Resources/examples/pxScene2d/external/node

test -d "${LOG_DIR}" || mkdir "${LOG_DIR}"
printf "Base directory is ${BASE_DIR}\n" > ${LOG_FILE} 2>&1
printf "Node directory is ${NODE_DIR}\n" > ${LOG_FILE} 2>&1
ER=`ps -Al | grep EngineRunner | grep -v grep`

d=`date`
printf "${d}: ${ER}\n" >> ${LOG_FILE} 2>&1
if [ -f "${AUTO_UPDATE}" ] && [ -z "${ER}" ]; then
   printf "Running EngineRunner (Auto update)\n"
   ${BASE_DIR}/EngineRunner run -productid ${PRODUCT_ID} -version ${PX_VERSION} -url ${UPDATE_URL} >> ${LOG_FILE} 2>&1 &
fi
export PATH=${NODE_DIR}/bin:${NODE_DIR}/lib/node_modules/npm/bin/node-gyp-bin:${PATH}
cd ${BASE_DIR}/../Resources/examples/pxScene2d/src/jsbindings
printf "Launching load.sh brower.js\n" >> ${LOG_FILE} 2>&1 
./load.sh browser.js > ${DEBUG_FILE} 2>&1
printf "load.sh browser.js done.\n" >> ${LOG_FILE} 2>&1
