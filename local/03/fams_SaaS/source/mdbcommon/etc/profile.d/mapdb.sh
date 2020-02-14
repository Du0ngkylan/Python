# COPYRIGHT 2017 FUJITSU LIMITED
# @file        mapdb.sh
# @brief       mapdb initialization script
# @author      wada
# @date        2016/10/25

BINDIR=/opt/FJSVmapdb/bin
CMD_GREP=/bin/grep

if ! echo ${PATH} | ${CMD_GREP} -q ${BINDIR} ; then
    PATH=${PATH}:${BINDIR}
fi

export PATH
