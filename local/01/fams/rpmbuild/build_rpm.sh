#!/bin/sh

# This script run on build server or local build machine

set -e #stop on error

MIOT_HOME=$(pwd)/../
RPM_HOME=$(pwd)
echo ">>>>>>>>>>>>>>>>"+"$BUILD_DIR"

echo "$MIOT_HOME"
#Server name
COMMON="mdb-com"
FAMS_MNG="mdb-fdm"
FAMS_DDB="mdb-fdb"

if [ -z "$1" ]; then
	echo "build_rpm.sh [params...]"
	echo "Params:"
	echo "  # [param1] = all/fdm/common/fdb"
	echo "  # [param2] = delete (optional, if want delete old rpms)"
	exit 0
fi

RPMVER="0.1.1"

#increase revision
REV=$(date '+%m%d%H%M')
#echo $REV

BUILD_DIR=${MIOT_HOME}
SOURCE_DIR=${MIOT_HOME}/source/

# create missing folder
mkdir -p ${MIOT_HOME}/source/mdbcommon/dev/mapdb

cd ${SOURCE_DIR}
echo "Current dir>>>>>>>>>>>>>>>>>>>>>"
echo `pwd`
# clean objects
make clean
#> /dev/null

if [ "$1" == "common" ] || [ "$1" == "all" ]; then
	BUILD_DIR=${PWD}/../rpmbuild/
	COMMON_TARNAME=${COMMON}-${RPMVER}
	if [ "$2" == "delete" ]; then
		rm -f ${BUILD_DIR}/RPMS/x86_64/${COMMON}-${RPMVER}* #remove local existing rpm
	fi
	cd ${SOURCE_DIR}
  echo "${BUILD_DIR}/SOURCES/${COMMON_TARNAME}.tar.gz"
  pwd
	tar zcvf ${BUILD_DIR}/SOURCES/${COMMON_TARNAME}.tar.gz mdbcommon  --exclude='*.o' --exclude='*.mes'  --exclude='*.fd'  --exclude='*.aa' --exclude='*.ao'  --exclude '.svn' 
	cp -a ${BUILD_DIR}/SPECS/${COMMON}.spec ${BUILD_DIR}/SPECS/${COMMON}.build.spec

	sed -i -r -e "s/###Ver###/${RPMVER}/g" ${BUILD_DIR}/SPECS/${COMMON}.build.spec
	sed -i -r -e "s/###Rev###/${REV}/g" ${BUILD_DIR}/SPECS/${COMMON}.build.spec

	cd ${BUILD_DIR}
	rpmbuild --define "_topdir `pwd`" -ba ${BUILD_DIR}/SPECS/${COMMON}.build.spec
	if [ $? != 0 ]; then
		rm -f ${BUILD_DIR}/SPECS/${COMMON}.build.spec
		exit 1
	fi
	rm -f ${BUILD_DIR}/SPECS/${COMMON}.build.spec
fi


if [ "$1" == "fdm" ] || [ "$1" == "all" ]; then
	BUILD_DIR=${PWD}/../rpmbuild/
	FAMS_MNG_TARNAME=${FAMS_MNG}-${RPMVER}
	if [ "$2" == "delete" ]; then
		rm -f ${BUILD_DIR}/RPMS/x86_64/${FAMS_MNG}-${RPMVER}* #remove local existing rpm
	fi

	cd ${SOURCE_DIR}
	tar zcvf ${BUILD_DIR}/SOURCES/${FAMS_MNG_TARNAME}.tar.gz mdbcommon famsdatamanagement --exclude='*.o' --exclude='*.mes'  --exclude='*.fd'  --exclude='*.aa' --exclude='*.ao' --exclude '.svn' 
	cp -a ${BUILD_DIR}/SPECS/${FAMS_MNG}.spec ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec
	sed -i -r -e "s/###Ver###/${RPMVER}/g" ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec
	sed -i -r -e "s/###Rev###/${REV}/g" ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec
	
	cd ${BUILD_DIR}
	#rpmbuild -ba  ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec
	rpmbuild --define "_topdir `pwd`" -ba ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec

	if [ $? != 0 ]; then
		rm -f ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec
		exit 1
	fi
	rm -f ${BUILD_DIR}/SPECS/${FAMS_MNG}.build.spec
fi

# rpm for db deletion
if [ "$1" == "fdb" ] || [ "$1" == "all" ]; then
	#REV=9999
	MDB_TARNAME=${FAMS_DDB}-${RPMVER}
	echo "${PWD}"
	BUILD_DIR=$RPM_HOME
	echo "$BUILD_DIR"
	if [ "$2" == "delete" ]; then
		rm -f ${BUILD_DIR}/RPMS/x86_64/${FAMS_DDB}-${RPMVER}* #remove local existing rpm
	fi

	cd ${SOURCE_DIR}
	tar zcvf ${BUILD_DIR}/SOURCES/${MDB_TARNAME}.tar.gz famsdboperator --exclude '.svn'

	cp -a ${BUILD_DIR}/SPECS/${FAMS_DDB}.spec ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec
	sed -i -r -e "s/###Ver###/${RPMVER}/g" ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec
	sed -i -r -e "s/###Rev###/${REV}/g" ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec

	rpmbuild --define "_topdir ${BUILD_DIR}" -ba ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec
#	rpmbuild -ba ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec
	if [ $? != 0 ]; then
		rm -f ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec
		exit 1
	fi
	rm -f ${BUILD_DIR}/SPECS/${FAMS_DDB}.build.spec
fi

rm -rf ${BUILD_DIR}/BUILDROOT/*
rm -rf ${BUILD_DIR}/BUILD/*
rm -rf ${BUILD_DIR}/SRPMS/*

echo "========================== FINISH BUILD================================"
echo "========================== rpms in: ==================================="
ls -la ${BUILD_DIR}/RPMS/x86_64/mdb*



