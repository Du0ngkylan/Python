# COPYRIGHT 2017 FUJITSU LIMITED
# @file        Makefile
# @brief       Make Configuration file
# @author      QNET
# @date        2016/12/7
# Command line
COMMANDS = \
	mdbdbgloginit \
	mdbdbglog \
	mdbencryptor \

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
	${CURDIR}/script/mapdb_functions \
	${CURDIR}/script/mdbpasswd \
	${CURDIR}/script/mdbutil \

# Common library
LIBMDB_DIR = $(ROOT_DIR)/lib/build

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
	libmdbdatabase \

# Use in Makefile
OBJ_FILES := \
	$(OBJ_DIR)/mdbdbgloginit.o \
	$(OBJ_DIR)/mdbdbglog.o \
	$(OBJ_DIR)/mdbencryptor.o \

OBJ_DEPENDENTS := \
	$(OBJ_DIR)/MDBLogConfiguration.o \

# Depenency include dir
INC_DIR = -I $(CURDIR)/include
INC_LIBMDB_DIR = -I $(ROOT_DIR)/include

INC_FLAGS = $(INC_DIR) $(INC_LIBMDB_DIR)

LIB_FLAGS = -L"$(LIBMDB_DIR)" $(addprefix -l, $(COMMON_LIB:lib%=%)) -lPocoFoundation -lPocoUtil -lPocoNet -lPocoCrypto -lpthread -lrt -ljansson -lpq

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
	@chmod +x $(BIN_DIR)/mdb*

$(OBJ_DIR)/%.o: $(CURDIR)/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) $(CCFLAGS) -c $(INC_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	@echo 'Building target: '
	$(CXX) $(CCFLAGS) -o $@ $< $(OBJ_DEPENDENTS) $(LIB_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '

clean:
	$(RM) $(TARGET) $(BIN_DIR) $(OBJ_DIR)

.PHONY: dependents
