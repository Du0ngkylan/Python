# COPYRIGHT 2017 FUJITSU LIMITED
# @file        Makefile
# @brief       Make Configuration file
# @author      QNET
# @date        2016/12/7

ROOT_DIR = $(abspath ${CURDIR}/)

all: debug

debug: MAKE_TARGET=debug
debug: lib
	MAKE_TARGET=$(MAKE_TARGET) make apps

release: MAKE_TARGET=release
release: lib
	MAKE_TARGET=$(MAKE_TARGET) make apps

eff: MAKE_TARGET=eff
eff: lib
	MAKE_TARGET=$(MAKE_TARGET) make apps

app: cleanapp
	make debug

apps: script

script:
	@echo "=== BUILDING COMMAND TOOLS ==="
	make -C ${ROOT_DIR}/script $(MAKE_TARGET)

lib:
	@echo "=== BUILDING COMMON LIB ==="
	make -C ${ROOT_DIR}/lib $(MAKE_TARGET)

cleanapp:
	make -C ${ROOT_DIR}/script clean
	make -C ${ROOT_DIR}/lib clean
	-@echo ' '

clean:
	@echo "=== CLEAN ==="
	make -C ${ROOT_DIR}/script clean
	make -C ${ROOT_DIR}/lib clean
	-@echo ' '


.PHONY: lib script clean 
