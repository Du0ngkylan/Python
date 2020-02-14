# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        Makefile
# @brief       Make file to build Data Utilization
# @author      anhnguyen
# @date        2018/12/10
# famsdatautil
DAEMON_NAME = famsdtul

RM := rm -rf

COMMON_DAEMON_DIR = $(abspath ${CURDIR}/../)

# output
SERVER_DIR = ${CURDIR}/../
SERVER_BUILD_DIR = ${SERVER_DIR}/build
SERVER_BIN_DIR = ${SERVER_BUILD_DIR}/bin
BUILD_DIR = ${CURDIR}/build
OBJ_DIR = ${BUILD_DIR}/obj
TARGET = $(SERVER_BIN_DIR)/$(DAEMON_NAME)

FAMS_COMMON_BUILD_DIR = $(COMMON_DAEMON_DIR)/famscommon/build

# Common library
COMMON_DIR = ../../../../../mdbcommon/opt/FJSVmapdb

ifdef AFTER_MAKE
  LIBFAMS_DIR = /opt/FJSVmapdb/lib
else
  LIBFAMS_DIR = $(COMMON_DIR)/lib/build/
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

# Use in Makefile
OBJ_FILES := \
       $(OBJ_DIR)/FAMSDURequestHandlerFactory.o \
       $(OBJ_DIR)/FAMSDUAppServer.o \
       $(OBJ_DIR)/FAMSDUConfig.o \
       $(OBJ_DIR)/FAMSDUUserHandler.o \
       $(OBJ_DIR)/FAMSDURqUserAnalyzer.o \
       $(OBJ_DIR)/FAMSDUObserversHandler.o \
       $(OBJ_DIR)/FAMSDURqObserversAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqCisternAnalyzer.o \
       $(OBJ_DIR)/FAMSDUBaseHandler.o \
	   $(OBJ_DIR)/FAMSDUCisternHandler.o \
       $(OBJ_DIR)/FAMSDUVersionHandler.o \
       $(OBJ_DIR)/FAMSDURqVersionAnalyzer.o \
       $(OBJ_DIR)/FAMSDUOrganismHandler.o \
       $(OBJ_DIR)/FAMSDURqOrganismAnalyzer.o \
       $(OBJ_DIR)/FAMSDUTaskHandler.o \
       $(OBJ_DIR)/FAMSDURqTaskDisposalInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskFeedingInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskMaintenanceInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskMonitorInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskPondInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskRecoveryInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskShippingInputAnalyzer.o \
	   $(OBJ_DIR)/FAMSDURqTaskWaterInputAnalyzer.o \
       $(OBJ_DIR)/FAMSDUCommon.o

FAMS_COMMON_OBJ_FILES = $(FAMS_COMMON_BUILD_DIR)/obj/FAMSCommon.o

# Depenency include dir
INC_DIR = -I $(CURDIR)/include
INC_LIBFAMS_DIR = -I $(COMMON_DIR)/include
INC_FAMS_COMMON_COM_DIR = -I$(COMMON_DAEMON_DIR)/famscommon/include

INC_FLAGS = $(INC_DIR) $(INC_LIBFAMS_DIR) $(INC_FAMS_COMMON_COM_DIR)

LIB_FLAGS = -L"$(LIBFAMS_DIR)" $(addprefix -l, $(COMMON_LIB:lib%=%)) -lPocoFoundation -lPocoUtil -lPocoNet -lPocoCrypto -lPocoNetSSL \
	-lpthread -lrt -ljansson -lpq -lyaml-cpp -Wl,--start-group -Wl,--end-group

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
release-independent: CCFLAGS = $(CC_BASE_FLAGS) $(CC_DEBUG_FLAGS)
release-independent: $(TARGET)

PHONY += eff eff-independent # avoid exists eff, eff-independent files/directories
eff: dependents
	make eff-independent
eff-independent: CCFLAGS = $(CC_BASE_FLAGS) $(CC_DEBUG_FLAGS) -Werror # build this to fix warning
eff-independent: $(TARGET)

PHONY += clean # avoid exists dependents file/directory
dependents:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(SERVER_BIN_DIR)

$(OBJ_DIR)/%.o: $(CURDIR)/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -c $(CCFLAGS) $(INC_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TARGET): $(OBJ_FILES) $(FAMS_COMMON_OBJ_FILES)
	@echo 'Building target:  $(TARGET)'
	$(CXX) $(CCFLAGS) -o $(TARGET) $(OBJ_FILES) $(FAMS_COMMON_OBJ_FILES) $(LIB_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '

PHONY += clean # avoid exists clean file/directory
clean:
	$(RM) $(TARGET) $(SERVER_BIN_DIR) $(OBJ_DIR)

.PHONY: $(PHONY)
