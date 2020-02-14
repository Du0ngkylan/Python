# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        Makefile
# @brief       Make Configuration file
# @author      anhnguyen
# @date        2018/12/12
# mdbdynamic
DAEMON_NAME = famsevtps

RM := rm -rf

ifeq ($(ROOT_DIR),)
	ROOT_DIR = $(abspath ${CURDIR}/../../)
endif

# output
SERVER_DIR = ${CURDIR}/../
SERVER_BUILD_DIR = ${SERVER_DIR}/build
SERVER_BIN_DIR = ${SERVER_BUILD_DIR}/bin
BUILD_DIR = ${CURDIR}/build
BIN_DIR = ${BUILD_DIR}/bin
OBJ_DIR = ${BUILD_DIR}/obj
TARGET = $(SERVER_BIN_DIR)/$(DAEMON_NAME)

DYNAMIC_DAEMON_DIR = $(abspath ${CURDIR}/../)
DYNAMIC_BUILD_DIR = $(DYNAMIC_DAEMON_DIR)/famscommon/build

# Common library
COMMON_DIR = $(ROOT_DIR)/../../../mdbcommon/opt/FJSVmapdb
ifdef AFTER_MAKE
  LIBDMAP_DIR = /opt/FJSVmapdb/lib
else
  LIBDMAP_DIR = $(COMMON_DIR)/lib/build/
endif

# common lib dependency
COMMON_LIB = \
	libmdbproc \
	libmdbconf \
	libmdbutil \
	libmdbhttp \
	libmdbsig \
	libmdbcrypto \
	libmdblog \
	libmdbshm \
	libmdbthread \
	libmdbinout \
	libmdbjson \
	libmdbdatabase \
	libmdbyaml \
	libmdbipc 

# Use in Makefile
OBJ_FILES := \
       $(OBJ_DIR)/FAMSEPConfig.o \
       $(OBJ_DIR)/FAMSEPCommon.o \
       $(OBJ_DIR)/FAMSEPApplication.o \
       $(OBJ_DIR)/FAMSEPEventHandler.o\
	   $(OBJ_DIR)/FAMSEPSalinityCalculation.o \
	   $(OBJ_DIR)/FAMSEPSensorHandler.o

DYNAMIC_OBJ_FILES = $(DYNAMIC_BUILD_DIR)/obj/FAMSCommon.o \
		$(DYNAMIC_BUILD_DIR)/obj/FAMSReceptionAndProcessingShmConfig.o \
		$(DYNAMIC_BUILD_DIR)/obj/FAMSIpcSensorData.o \

# Depenency include dir
INC_DIR = -I $(CURDIR)/include
INC_LIBCOMMON_DIR = -I $(CURDIR)/../famscommon/include
INC_LIBDMAP_DIR = -I $(COMMON_DIR)/include

INC_FLAGS = $(INC_DIR) $(INC_LIBDMAP_DIR) $(INC_LIBCOMMON_DIR)

LIB_FLAGS = $(LIB_ZENRIN_DIR) -L"$(LIBDMAP_DIR)" $(addprefix -l, $(COMMON_LIB:lib%=%)) -lPocoFoundation -lPocoUtil -lPocoNetSSL -lPocoNet -lPocoCrypto \
-lpthread -lrt -ljansson -lpq -lyaml-cpp

CC_RELEASE_FLAGS ?= -O1 -D_FORTIFY_SOURCE=2
CC_DEBUG_FLAGS ?= -O0 -g3 -DDEBUG
CC_BASE_FLAGS ?= -std=c++11 -Wall -W -Weffc++ -fPIC -D_LINUX -Wformat=2 -D__STDC_FORMAT_MACROS

all: debug

PHONY += debug debug-independent # avoid exists debug, debug-independent files/directories
debug: dependents
	make debug-independent
debug-independent: CCFLAGS = $(CC_BASE_FLAGS) $(CC_DEBUG_FLAGS)
debug-independent: $(TARGET)

PHONY += release release-independent # avoid exists release, release-independent files/directories
release: dependents
	make release-independent
release-independent: CCFLAGS = $(CC_BASE_FLAGS) $(CC_RELEASE_FLAGS)
release-independent: $(TARGET)

PHONY += eff eff-independent # avoid exists eff, eff-independent files/directories
eff: dependents
	make eff-independent
eff-independent: CCFLAGS = $(CC_BASE_FLAGS) $(CC_DEBUG_FLAGS) -Werror
eff-independent: $(TARGET)

PHONY += clean # avoid exists dependents file/directory
dependents:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(SERVER_BIN_DIR)

$(OBJ_DIR)/%.o: $(CURDIR)/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@echo 'ROOT DIR >>>>>>>>>>>>>>>>>:'
	@echo  $(ROOT_DIR)
	@echo 'INC_FLAGS >>>>>>>>>>>>>>>>>:'
	@echo  $(INC_FLAGS)

	$(CXX) -c $(CCFLAGS) $(INC_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TARGET): $(OBJ_FILES) $(DYNAMIC_OBJ_FILES)
	@echo 'Building target:  $(TARGET)'
	$(CXX) $(CCFLAGS) -o $(TARGET) $(OBJ_FILES) $(DYNAMIC_OBJ_FILES) $(LIB_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '

PHONY += clean # avoid exists clean file/directory
clean:
	$(RM) $(TARGET) $(BIN_DIR) $(OBJ_DIR)

.PHONY: $(PHONY)
