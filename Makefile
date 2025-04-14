PROJECT := keyboard_bootloader

TARGET ?= MAX32650
TARGET_UC := $(shell echo $(TARGET) | tr '[:lower:]' '[:upper:]')
TARGET_LC := $(shell echo $(TARGET) | tr '[:upper:]' '[:lower:]')
export TARGET
export TARGET_UC
export TARGET_LC

BOARD ?= keyboard

ifeq ($(strip $(MAXIM_PATH)),)
$(error MAXIM_PATH not defined.)
endif

LIBS_DIR := $(abspath $(MAXIM_PATH)/Libraries)
CMSIS_ROOT := $(LIBS_DIR)/CMSIS
export CMSIS_ROOT

COMPILER := GCC
ifneq ($(strip $(DEBUG)),)
MXC_OPTIMIZE_CFLAGS += -O0
PROJ_CFLAGS += -g -DDEBUG
else
MXC_OPTIMIZE_CFLAGS += -Os
endif
export MXC_OPTIMIZE_CFLAGS

PATH := $(MAXIM_PATH)/Tools/GNUTools/10.3/bin:$(PATH)

MFLOAT_ABI ?= softfp
export MFLOAT_ABI

PROJ_CFLAGS += -Wall
PROJ_CFLAGS += -DMXC_ENABLE
PROJ_CFLAGS += -DNO_EVAL_FEATURES
PROJ_CFLAGS += -fdiagnostics-color=always

CROSS_COMPILE ?= arm-none-eabi
CC := $(CROSS_COMPILE)-gcc
LD := $(CROSS_COMPILE)-ld
OBJCOPY := $(CROSS_COMPILE)-objcopy
OBJDUMP := $(CROSS_COMPILE)-objdump

VPATH += src
VPATH += src/dfu
VPATH += src/usb
VPATH += device/$(TARGET_LC)/src
VPATH += device/$(TARGET_LC)/src/usb
VPATH += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source
VPATH := $(VPATH)
SRCS := $(wildcard $(addsuffix /*.c, $(VPATH)))

IPATH += .
IPATH += include
IPATH += device/$(TARGET_LC)/include

SRC_DIR ?= src
BLD_DIR ?= build

ENTRY := flash_reset_handler
LINKERFILE := device/$(TARGET_LC)/linker.ld
STARTUPFILE := device/$(TARGET_LC)/src/startup.c

LIB_BOARD := 0
LIB_MAXUSB := 1
include $(LIBS_DIR)/libs.mk
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk

ifeq ($(strip $(MAKECMDGOALS)),)
MAKECMDGOALS := all
endif

.PHONY: all clean libclean distclean

$(BLD_DIR)/$(PROJECT).dasm: FORCE

$(BLD_DIR)/$(PROJECT).bin: FORCE

$(BLD_DIR)/$(PROJECT).dfu: FORCE

$(BLD_DIR)/$(PROJECT).dfu: $(BLD_DIR)/$(PROJECT).bin
	cp $< $@
	dfu-suffix -v 0xDEAD -p 0xBEEF --add $@

all: $(BLD_DIR)/$(PROJECT).dasm $(BLD_DIR)/$(PROJECT).dfu
	arm-none-eabi-size --format=berkeley $(BUILD_DIR)/$(PROJECT).elf

clean:
	$(RM) -r $(BLD_DIR)

libclean: 
	$(MAKE) -f $(PERIPH_DRIVER_DIR)/periphdriver.mk clean.periph

distclean: clean libclean

print-%:
	@echo $* = $($*)
