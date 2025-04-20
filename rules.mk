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
	$(LD) $(LDFLAGS) -o $@ $(filter %.o,$^) --start-group $(filter %.a,$^) $(LIBS) --end-group

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.dasm: %.elf
	$(OBJDUMP) -S $< > $@

%.dfu: %.bin
	cp $< $@
	dfu-suffix -v 0xDEAD -p 0xBEEF --add $@
