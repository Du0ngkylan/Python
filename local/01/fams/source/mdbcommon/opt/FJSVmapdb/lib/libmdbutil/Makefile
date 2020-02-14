# COPYRIGHT 2017 FUJITSU LIMITED
# @file        Makefile
# @brief       Make Configuration file
# @author      QNET
# @date        2016/12/7
LIBRARY_NAME = libmdbutil

ifeq ($(ROOT_DIR),)
	ROOT_DIR = $(abspath ${CURDIR}/../../)
endif

TARGET = $(ROOT_DIR)/lib/build/$(LIBRARY_NAME).so

RM := rm -rf

# Use in Makefile
OBJ_FILES := \
	$(CURDIR)/build/MDBConstants.o \
	$(CURDIR)/build/MDBException.o \
	$(CURDIR)/build/MDBCommonUtility.o \

# Depenency include dir
INC_LIB_DIR = -I $(CURDIR)/include
INC_COMMON_LIB_DIR = -I $(ROOT_DIR)/include

INC_FLAGS = $(INC_LIB_DIR) $(INC_COMMON_LIB_DIR)

CC_RELEASE_FLAGS ?= -O1 -D_FORTIFY_SOURCE=2
CC_DEBUG_FLAGS ?= -O0 -g3 -DDEBUG
CC_BASE_FLAGS ?= -std=c++11 -Wall -W -Weffc++ -fPIC -D_LINUX -Wformat=2

all: debug

dependents:
	mkdir -p $(CURDIR)/build

debug: CCFLAGS=$(CC_DEBUG_FLAGS) $(CC_BASE_FLAGS) -c
debug: dependents $(TARGET)

release: CCFLAGS=$(CC_RELEASE_FLAGS) $(CC_BASE_FLAGS) -c
release: dependents $(TARGET)

eff: CCFLAGS=$(CC_DEBUG_FLAGS) $(CC_BASE_FLAGS) -Werror -c
eff: dependents $(TARGET)

$(CURDIR)/build/%.o: $(CURDIR)/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) $(CCFLAGS) $(INC_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TARGET): $(OBJ_FILES)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CXX) -shared -o "$@" $(OBJ_FILES)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	$(RM) $(OBJ_FILES) $(TARGET)
