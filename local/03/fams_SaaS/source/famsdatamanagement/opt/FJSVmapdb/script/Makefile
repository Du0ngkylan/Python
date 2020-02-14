# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        Makefile
# @brief       Make Configuration file
# @author      anhnguyen
# @date        2018/12/10
# Command line
COMMANDS = \

RM := rm -rf

ifeq ($(ROOT_DIR),)
	ROOT_DIR = $(abspath ${CURDIR}/../)
endif

# output
BIN_DIR = ${CURDIR}/build/bin
OBJ_DIR = ${CURDIR}/build/obj
SRC_DIR = ${CURDIR}/src
TARGET = $(addprefix $(BIN_DIR)/, $(COMMANDS))
SCRIPTS = \
	${CURDIR}/script/mdbdatamanagementadm \

# Common library
LIBFAMS_DIR = $(ROOT_DIR)/lib/build

# common lib dependency
COMMON_LIB = \

# Use in Makefile
OBJ_FILES := \

OBJ_DEPENDENTS := \

# Depenency include dir
INC_DIR = -I $(CURDIR)/include
INC_LIBFAMS_DIR = -I $(ROOT_DIR)/include

INC_FLAGS = $(INC_DIR) $(INC_LIBFAMS_DIR)

LIB_FLAGS = -L"$(LIBFAMS_DIR)" $(addprefix -l, $(COMMON_LIB:lib%=%)) -lPocoFoundation -lPocoUtil -lPocoNet -lPocoCrypto -lpthread -lrt -ljansson -lpq

CC_RELEASE_FLAGS ?= -O1 -D_FORTIFY_SOURCE=2
CC_DEBUG_FLAGS ?= -O0 -g3 -DDEBUG
CC_BASE_FLAGS ?= -std=c++11 -Wall -W -Weffc++ -fPIC -D_LINUX -Wformat=2

all: debug

debug: dependents scripts
	make debug-independent

debug-independent: MAKE_TARGET=debug
debug-independent: CCFLAGS=$(CC_BASE_FLAGS) $(CC_DEBUG_FLAGS)
debug-independent: $(OBJ_FILES) $(OBJ_DEPENDENTS) $(TARGET)

release: dependents scripts
	make release-independent

release-independent: MAKE_TARGET=release
release-independent: CCFLAGS=$(CC_BASE_FLAGS) $(CC_RELEASE_FLAGS)
release-independent: $(OBJ_FILES) $(OBJ_DEPENDENTS) $(TARGET)

eff: dependents scripts
	make eff-independent

eff-independent: MAKE_TARGET=eff
eff-independent: CCFLAGS=$(CC_BASE_FLAGS) $(CC_DEBUG_FLAGS) -Werror
eff-independent: $(OBJ_FILES) $(OBJ_DEPENDENTS) $(TARGET)

dependents:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

scripts:
	@echo 'Building scripts'
	@cp $(SCRIPTS) $(BIN_DIR)
	@chmod +x $(BIN_DIR)/*

clean:
	$(RM) $(TARGET) $(BIN_DIR) $(OBJ_DIR)

.PHONY: dependents
