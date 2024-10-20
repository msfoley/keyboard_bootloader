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
MXC_OPTIMIZE_CFLAGS += -Og -g
else
MXC_OPTIMIZE_CFLAGS += -O2
endif

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
VPATH += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source
VPATH := $(VPATH)
SRCS := $(wildcard $(addsuffix /*.c, $(VPATH)))

IPATH += .
IPATH += include
IPATH += include/$(TARGET_LC)

SRC_DIR ?= src
BLD_DIR ?= build

ENTRY := flash_reset_handler
LINKERFILE := include/$(TARGET_LC)/linker.ld
STARTUPFILE := src/startup.c

LIB_BOARD := 0
LIB_MAXUSB := 1
include $(LIBS_DIR)/libs.mk
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk

ifeq ($(strip $(MAKECMDGOALS)),)
MAKECMDGOALS := all
endif

.PHONY: all clean libclean distclean

$(BLD_DIR)/$(PROJECT).dasm: $(BLD_DIR)/$(PROJECT).elf

all: $(BLD_DIR)/$(PROJECT).bin $(BLD_DIR)/$(PROJECT).dasm
	arm-none-eabi-size --format=berkeley $(BUILD_DIR)/$(PROJECT).elf

clean:
	$(RM) -r $(BLD_DIR)

libclean: 
	$(MAKE) -f $(PERIPH_DRIVER_DIR)/periphdriver.mk clean.periph

distclean: clean libclean

print-%:
	@echo $* = $($*)
