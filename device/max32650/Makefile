include config.mk

COMPILER ?= GCC

ifeq ($(strip $(MAXIM_PATH)),)
$(error MAXIM_PATH not defined.)
endif
PATH := $(MAXIM_PATH)/Tools/GNUTools/10.3/bin:$(PATH)

LIB_PATH := $(MAXIM_PATH)/Libraries

TARGET := MAX32650

SUBMAKE_ARGS += CC=$(CC) LD=$(CC) AR=$(AR) OBJCOPY=$(OBJCOPY) OBJDUMP=$(OBJDUMP)
SUBMAKE_ARGS += MXC_OPTIMIZE_FLAGS=$(OPT_FLAGS)
SUBMAKE_ARGS += MFLOAT_ABI=$(MFLOAT_ABI) MFPU=$(MFPU)
SUBMAKE_ARGS += TARGET=$(TARGET) COMPILER=$(COMPILER)
ifneq ($(strip $(DEBUG)),)
SUBMAKE_ARGS += DEBUG=1
endif

lib: $(BLD_DIR)/libPeriphDriver.a $(BLD_DIR)/maxusb.a

clean:
	$(RM) -r $(BLD_DIR)

$(BLD_DIR)/libPeriphDriver.a:
	mkdir -p $(BLD_DIR)/periphDriver
	cd $(LIB_PATH)/PeriphDrivers && $(MAKE) -f $(LIB_PATH)/PeriphDrivers/libPeriphDriver.mk BUILD_DIR=$(BLD_DIR)/periphDriver $(SUBMAKE_ARGS)
	$(AR) -cr $@ $(BLD_DIR)/periphDriver/*.o

$(BLD_DIR)/maxusb.a:
	mkdir -p $(BLD_DIR)
	$(MAKE) -C $(LIB_PATH)/MAXUSB BUILD_DIR=$(BLD_DIR)/usb MAXUSB_DIR=$(LIB_PATH)/MAXUSB PERIPH_DRIVER_DIR=$(LIB_PATH)/PeriphDrivers/$(TARGET_UC) $(SUBMAKE_ARGS) lib
	cp $(BLD_DIR)/usb/maxusb.a $@
