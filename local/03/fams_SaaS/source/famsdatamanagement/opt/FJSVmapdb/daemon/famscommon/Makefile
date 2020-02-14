# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        Makefile
# @brief       Make Configuration file
# @author      ThangLQ
# @date        2020/01/20
# famscommon
TARGET = famscommon

RM := rm -rf

ifeq ($(ROOT_DIR),)
	ROOT_DIR = $(abspath ${CURDIR}/../../)
endif

# output
BUILD_DIR = ${CURDIR}/build
OBJ_DIR = ${BUILD_DIR}/obj

# Common library
COMMON_DIR = $(ROOT_DIR)/../../../mdbcommon/opt/FJSVmapdb
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
       $(OBJ_DIR)/FAMSCommon.o \
       $(OBJ_DIR)/FAMSReceptionAndProcessingShmConfig.o \
	   $(OBJ_DIR)/FAMSIpcSensorData.o \

# Depenency include dir
INC_DIR = -I $(CURDIR)/include

INC_LIBFAMS_DIR = -I $(COMMON_DIR)/include

INC_FLAGS = $(INC_DIR) $(INC_LIBFAMS_DIR) $(INC_LIBDATA_DIR)
#g++  -o "test"  ./src/test.o   -ljansson -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lPocoCrypto
LIB_FLAGS = -L"$(LIBFAMS_DIR)" $(addprefix -l, $(COMMON_LIB:lib%=%)) -lpthread -lrt -ljansson -lpq -lyaml-cpp -lPocoNet -lPocoFoundation -lPocoUtil -lPocoCrypto -lPocoNetSSL

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
	@mkdir -p $(OBJ_DIR)

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

$(TARGET): $(OBJ_FILES)
	@echo 'Building target:  $(TARGET)'
	@echo 'Finished building: $<'
	@echo ' '

PHONY += clean # avoid exists clean file/directory
clean:
	$(RM) $(TARGET) $(OBJ_DIR)

.PHONY: $(PHONY)
