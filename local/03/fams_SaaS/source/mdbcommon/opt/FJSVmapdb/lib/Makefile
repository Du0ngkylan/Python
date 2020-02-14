# COPYRIGHT 2017 FUJITSU LIMITED
# @file        Makefile
# @brief       Make Configuration file
# @author      QNET
# @date        2016/12/7
SUBDIRS = \
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
	libmdbipc \
	libmdbyaml \

ifeq ($(ROOT_DIR),)
	ROOT_DIR = $(abspath ${CURDIR}/../)
endif

LIB_BUILD_DIR = $(CURDIR)/build/
LIB_PROJECT_DIR = $(LIB_BUILD_DIR)
INC_PROJECT_DIR = $(ROOT_DIR)/include/
INC_LIB_DIR = $(addprefix $(CURDIR)/, $(addsuffix /include/*,$(SUBDIRS)))

RM := rm -rf

all: debug

debug: MAKE_TARGET=debug
debug: dependents $(SUBDIRS)

release: MAKE_TARGET=release
release: dependents $(SUBDIRS)

eff: MAKE_TARGET=eff
eff: dependents $(SUBDIRS)

PHONY := dependents

dependents:
	@mkdir -p $(LIB_BUILD_DIR)
	@mkdir -p $(LIB_PROJECT_DIR)
	@mkdir -p $(INC_PROJECT_DIR)

PHONY += $(SUBDIRS)
libmdbproc:
	@echo '== make libmdbproc'
	make -C ${ROOT_DIR}/lib/libmdbproc $(MAKE_TARGET)
libmdbconf:
	@echo '== make libmdbconf'
	make -C ${ROOT_DIR}/lib/libmdbconf $(MAKE_TARGET)
libmdbutil:
	@echo '== make libmdbutil'
	make -C ${ROOT_DIR}/lib/libmdbutil $(MAKE_TARGET)
libmdbhttp:
	@echo '== make libmdbhttp'
	make -C ${ROOT_DIR}/lib/libmdbhttp $(MAKE_TARGET)
libmdbsig:
	@echo '== make libmdbsig'
	make -C ${ROOT_DIR}/lib/libmdbsig $(MAKE_TARGET)
libmdbcrypto:
	@echo '== make libmdbcrypto'
	make -C ${ROOT_DIR}/lib/libmdbcrypto $(MAKE_TARGET)
libmdblog:
	@echo '== make libmdblog'
	make -C ${ROOT_DIR}/lib/libmdblog $(MAKE_TARGET)
libmdbshm:
	@echo '== make libmdbshm'
	make -C ${ROOT_DIR}/lib/libmdbshm $(MAKE_TARGET)
libmdbthread:
	@echo '== make libmdbthread'
	make -C ${ROOT_DIR}/lib/libmdbthread $(MAKE_TARGET)
libmdbinout:
	@echo '== make libmdbinout'
	make -C ${ROOT_DIR}/lib/libmdbinout $(MAKE_TARGET)
libmdbjson:
	@echo '== make libmdbjson'
	make -C ${ROOT_DIR}/lib/libmdbjson $(MAKE_TARGET)
libmdboverride:
	@echo '== make libmdboverride'
	make -C ${ROOT_DIR}/lib/libmdboverride $(MAKE_TARGET)
libmdbdatabase:
	@echo '== make libmdbdatabase'
	make -C ${ROOT_DIR}/lib/libmdbdatabase $(MAKE_TARGET)
libmdbsyslog:
	@echo '== make libmdbsyslog'
	make -C ${ROOT_DIR}/lib/libmdbsyslog $(MAKE_TARGET)
libmdbipc:
	@echo '== make libmdbipc'
	make -C ${ROOT_DIR}/lib/libmdbipc $(MAKE_TARGET)
libmdbyaml:
	@echo '== make libmdbyaml'
	make -C ${ROOT_DIR}/lib/libmdbyaml $(MAKE_TARGET)

PHONY += clean
clean:
	make -C ${ROOT_DIR}/lib/libmdbproc clean
	make -C ${ROOT_DIR}/lib/libmdbconf clean
	make -C ${ROOT_DIR}/lib/libmdbutil clean
	make -C ${ROOT_DIR}/lib/libmdbhttp clean
	make -C ${ROOT_DIR}/lib/libmdbsig clean
	make -C ${ROOT_DIR}/lib/libmdbcrypto clean
	make -C ${ROOT_DIR}/lib/libmdblog clean
	make -C ${ROOT_DIR}/lib/libmdbshm clean
	make -C ${ROOT_DIR}/lib/libmdbthread clean
	make -C ${ROOT_DIR}/lib/libmdbinout clean
	make -C ${ROOT_DIR}/lib/libmdbjson clean
	make -C ${ROOT_DIR}/lib/libmdbdatabase clean
	make -C ${ROOT_DIR}/lib/libmdbipc clean
	make -C ${ROOT_DIR}/lib/libmdbyaml clean
	$(RM) $(LIB_PROJECT_DIR)
	-@echo ' '

.PHONY: $(PHONY)
