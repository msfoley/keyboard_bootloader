$(BLD_DIR):
	mkdir -p $@

$(BLD_DIR)/%.o: %.c | $(BLD_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BLD_DIR)/%.o: %.s | $(BLD_DIR)
	@mkdir -p $(@D)
	$(CC) $(AFLAGS) -o $@ -c $<

$(BLD_DIR)/%.a: | $(BLD_DIR)
	$(AR) -cr $@ $^

$(BLD_DIR)/%.elf: | $(BLD_DIR)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,--start-group $(filter %.a,$^) $(LIBS) -Wl,--end-group

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.dasm: %.elf
	$(OBJDUMP) -S $< > $@

%.dfu: %.bin
	cp $< $@
	dfu-suffix -v 0xDEAD -p 0xBEEF --add $@
