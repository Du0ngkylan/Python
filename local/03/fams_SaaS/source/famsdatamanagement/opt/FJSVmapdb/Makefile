# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        Makefile
# @brief       Make Configuration file
# @author      anhnguyen
# @date        2018/12/10

ROOT_DIR = $(abspath ${CURDIR}/)

all: debug

debug: MAKE_TARGET=debug
debug: daemon
	MAKE_TARGET=$(MAKE_TARGET) make apps

release: MAKE_TARGET=release
release: daemon
	MAKE_TARGET=$(MAKE_TARGET) make apps

eff: MAKE_TARGET=eff
eff: daemon
	MAKE_TARGET=$(MAKE_TARGET) make apps

app: cleanapp
	make debug

apps: script daemon

daemon:
	@echo "=== BUILDING DAEMON ==="
	make -C ${ROOT_DIR}/daemon $(MAKE_TARGET)

script:
	@echo "=== BUILDING COMMAND TOOLS ==="
	make -C ${ROOT_DIR}/script $(MAKE_TARGET)

cleanapp:
	make -C ${ROOT_DIR}/daemon clean
	make -C ${ROOT_DIR}/script clean
	-@echo ' '

clean:
	make -C ${ROOT_DIR}/daemon clean
	make -C ${ROOT_DIR}/script clean
	-@echo ' '

.PHONY: daemon script
