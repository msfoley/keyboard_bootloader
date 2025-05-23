TARGET_UC := $(shell echo $(TARGET) | tr '[:lower:]' '[:upper:]')
TARGET_LC := $(shell echo $(TARGET) | tr '[:upper:]' '[:lower:]')

CROSS_COMPILE ?= arm-none-eabi

MFLOAT_ABI ?= softfp
MFPU ?= fpv4-sp-d16

COMMON_FLAGS += -mthumb
COMMON_FLAGS += -mcpu=cortex-m4
COMMON_FLAGS += -mfloat-abi=$(MFLOAT_ABI)
COMMON_FLAGS += -mfpu=$(MFPU)

CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wa,-mimplicit-it=thumb
CFLAGS += -D$(TARGET_UC)
CFLAGS += -DMXC_ENABLE
CFLAGS += -DNO_EVAL_FEATURES
CFLAGS += -DTARGET=$(TARGET)
CFLAGS += -D$(TARGET_UC)
CFLAGS += -DTARGET_REV=0x4131

LDFLAGS += -Wl,--gc-sections

ENTRY := flash_reset_handler

SUBMAKE_ARGS += MAXIM_PATH=$(MAXIM_PATH)

ifeq ($(strip $(MAXIM_PATH)),)
$(error MAXIM_PATH not defined.)
endif
PATH := $(MAXIM_PATH)/Tools/GNUTools/10.3/bin:$(PATH)

IPATH += $(MAXIM_PATH)/Libraries/PeriphDrivers/Include/$(TARGET_UC)
IPATH += $(MAXIM_PATH)/Libraries/MAXUSB/include/core/musbhsfc
IPATH += $(MAXIM_PATH)/Libraries/MAXUSB/include
IPATH += $(MAXIM_PATH)/Libraries/MAXUSB/include/core
IPATH += $(MAXIM_PATH)/Libraries/MAXUSB/include/enumerate
IPATH += $(MAXIM_PATH)/Libraries/MAXUSB/include/devclass
IPATH += $(MAXIM_PATH)/Libraries/MAXUSB/include/dbg_log
IPATH += $(MAXIM_PATH)/Libraries/CMSIS/Device/Maxim/MAX32650/Include
IPATH += $(MAXIM_PATH)/Libraries/CMSIS/Include

LIB_DEP := $(LIB_DIR)/libPeriphDriver.a $(LIB_DIR)/maxusb.a
