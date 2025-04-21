PROJECT := keyboard_bootloader

TARGET ?= max32650

SRC_DIR := src
BLD_DIR ?= build
LIB_DIR ?= lib

TARGET_DIR := device/$(TARGET)

C_SRCS := $(shell find $(SRC_DIR) -name "*.c")
C_SRCS += $(shell find $(TARGET_DIR)/src -name "*.c")
C_OBJS := $(patsubst %.c,$(BLD_DIR)/%.o,$(C_SRCS))

S_SRCS := $(shell find $(SRC_DIR) -name "*.s")
S_SRCS += $(shell find $(TARGET_DIR)/src -name "*.s")
S_OBJS := $(patsubst %.s,$(BLD_DIR)/%.o,$(C_SRCS))

SRCS := $(C_SRCS) $(S_SRCS)
OBJS := $(C_OBJS) $(S_OBJS)

IPATH += include
IPATH += $(TARGET_DIR)/include

include $(TARGET_DIR)/config.mk
include defs.mk

LIBS += -lc -lm -lnosys

C_WARNINGS_AS_ERRORS ?= implicit-function-declaration

ifneq ($(strip $(DEBUG)),)
OPT_FLAG := -O0
CFLAGS += -g -DDEBUG
SUBMAKE_ARGS += DEBUG=1
endif
OPT_FLAG ?= -Os

COMMON_FLAGS ?=

CFLAGS += $(COMMON_FLAGS)
CFLAGS += -Wall
CFLAGS += -std=gnu11
CFLAGS += $(OPT_FLAG)
CFLAGS += -fdiagnostics-color=always
CFLAGS += -Werror=$(C_WARNINGS_AS_ERRORS)
CFLAGS += -MD
CFLAGS += $(addprefix -I,$(IPATH))

AFLAGS += $(COMMON_FLAGS)
AFLAGS += -MD
AFLAGS += $(addprefix -I,$(IPATH))

LDFLAGS += -T $(TARGET_DIR)/linker.ld
LDFLAGS += --entry $(ENTRY)
LDFLAGS += $(COMMON_FLAGS)
LDFLAGS += $(addprefix -L,$(LIBPATH))
LDFLAGS += -Wl,-Map,$(BLD_DIR)/$(PROJECT).map
LDFLAGS += -Wl,--print-memory-usage

SUBMAKE_ARGS += BLD_DIR=$(abspath $(LIB_DIR))
SUBMAKE_ARGS += CC=$(CC) LD=$(LD) AR=$(AR) OBJCOPY=$(OBJCOPY) OBJDUMP=$(OBJDUMP)
SUBMAKE_ARGS += OPT_FLAG=$(OPT_FLAG)
SUBMAKE_ARGS += MFLOAT_ABI=$(MFLOAT_ABI) MFPU=$(MFPU)

.PHONY: all clean lib libclean distclean
.NOTINTERMEDIATE: $(BLD_DIR)/$(PROJECT).elf $(BLD_DIR)/$(PROJECT).bin

all: $(BLD_DIR)/$(PROJECT).dfu $(BLD_DIR)/$(PROJECT).dasm

include rules.mk

lib: $(LIB_DEP)

$(LIB_DEP):
	$(MAKE) -C $(TARGET_DIR) $(SUBMAKE_ARGS)

libclean:
	$(RM) -r $(LIB_DIR)

clean:
	$(RM) -r $(BLD_DIR)

distclean: clean libclean

$(BLD_DIR)/$(PROJECT).elf: $(OBJS) $(LIB_DEP)

print-%:
	@echo $* = $($*)

print-lib-%:
	make -C $(TARGET_DIR) $(SUBMAKE_ARGS) print-$*
