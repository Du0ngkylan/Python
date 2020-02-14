# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        Makefile
# @brief       Make Configuration file
# @author      anhnguyen
# @date        2018/12/10

# famsdatautilization daemons
DAEMONS = \
	famsdatareception \
	famseventprocessing \

AP_DAEMON = \
	famsdatautilization

COMMON = \
	famscommon \

CXX = g++
RM := rm -rf

PHONY :=
ifeq ($(ROOT_DIR),)
	ROOT_DIR = $(abspath ${CURDIR}/../)
endif

all: debug debug-ap
reception-processing: debug
ap: debug-ap

################## DATA RECEIVER & DATA UTILIZATION #########################
PHONY += debug debug-independent
debug: dependents
	make debug-independent
debug-independent: MAKE_TARGET=debug
debug-independent: $(DAEMONS) $(AP_DAEMON)

PHONY += release release-independent
release: dependents
	make release-independent
release-independent: MAKE_TARGET=release
release-independent: $(DAEMONS) $(AP_DAEMON)

PHONY += eff eff-independent
eff: dependents
	make eff-independent
eff-independent: MAKE_TARGET=eff
eff-independent: $(DAEMONS) $(AP_DAEMON)
################## DATA RECEIVER & DATA UTILIZATION #########################


###################### AP MANAGEMENT ############################
PHONY += debug-ap debug-independent-ap
debug-ap: dependents
	make debug-independent-ap
debug-independent-ap: MAKE_TARGET=debug
debug-independent-ap: $(AP_DAEMON)

PHONY += release-ap release-independent-ap
release-ap: dependents
	make release-independent-ap
release-independent-ap: MAKE_TARGET=release
release-independent-ap: $(AP_DAEMON)

PHONY += eff-ap eff-independent-ap
eff-ap: dependents
	make eff-independent-ap
eff-independent-ap: MAKE_TARGET=eff
eff-independent-ap: $(AP_DAEMON)
###################### AP MANAGEMENT ############################


PHONY += dependents
PHONY += $(COMMON)
dependents: $(COMMON)
	@echo "== make dependents"

PHONY += $(DAEMONS) $(AP_DAEMON)
famsdatareception:
	@echo "== make famsdatareception"
	make -C $(CURDIR)/famsdatareception $(MAKE_TARGET)

famseventprocessing:
	@echo "== make famseventprocessing"
	make -C $(CURDIR)/famseventprocessing $(MAKE_TARGET)

famsdatautilization:
	@echo "== make famsdatautilization"
	make -C $(CURDIR)/famsdatautilization $(MAKE_TARGET)

famscommon:
	@echo "== make famscommon"
	make -C $(CURDIR)/famscommon $(MAKE_TARGET)

PHONY += clean
clean:
	make -C ${CURDIR}/famscommon clean
	make -C ${CURDIR}/famsdatareception clean
	make -C ${CURDIR}/famseventprocessing clean
	make -C ${CURDIR}/famsdatautilization clean
	-@echo ' '

.PHONY: $(PHONY)
