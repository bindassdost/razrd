# comment out or override if you want to see the full output of each command
NOECHO ?= @

$(OUTBIN): $(OUTELF)
	@echo generating image: $@
	$(NOECHO)$(SIZE) $<
	$(NOCOPY)$(OBJCOPY) -O binary $< $@
	$(NOECHO)cp -f $@ ./build-$(PROJECT)/lk-no-mtk-header.bin
	../../../mediatek/build/tools/mkimage $@ LK > ./build-$(PROJECT)/lk_header.bin
	$(NOECHO)mv ./build-$(PROJECT)/lk_header.bin $@

ifeq ($(ENABLE_TRUSTZONE), 1)
$(OUTELF): $(ALLOBJS) $(LINKER_SCRIPT) $(OUTPUT_TZ_BIN)
ifeq ($(BUILD_SEC_LIB),yes)
	@echo delete old security library
	@rm -rf $(LK_TOP_DIR)/app/mt_boot/lib/libauth.a
	@rm -rf $(LK_TOP_DIR)/app/mt_boot/lib/libsec.a
	@echo linking security library
	@ar cq $(LK_TOP_DIR)/app/mt_boot/lib/libauth.a $(AUTH_OBJS)
	@ar cq $(LK_TOP_DIR)/app/mt_boot/lib/libsec.a $(SEC_OBJS)
endif
	@echo linking $@
	$(NOECHO)$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $(OUTPUT_TZ_BIN) $(ALLOBJS) $(LIBGCC) $(LIBSEC) -o $@
else
$(OUTELF): $(ALLOBJS) $(LINKER_SCRIPT)
ifeq ($(BUILD_SEC_LIB),yes)
	@echo delete old security library
	@rm -rf $(LK_TOP_DIR)/app/mt_boot/lib/libauth.a
	@rm -rf $(LK_TOP_DIR)/app/mt_boot/lib/libsec.a
	@echo linking security library
	@ar cq $(LK_TOP_DIR)/app/mt_boot/lib/libauth.a $(AUTH_OBJS)
	@ar cq $(LK_TOP_DIR)/app/mt_boot/lib/libsec.a $(SEC_OBJS)
endif
	@echo linking $@
	$(NOECHO)$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $(ALLOBJS) $(LIBGCC) $(LIBSEC) -o $@
endif

$(OUTELF).sym: $(OUTELF)
	@echo generating symbols: $@
	$(NOECHO)$(OBJDUMP) -t $< | $(CPPFILT) > $@

$(OUTELF).lst: $(OUTELF)
	@echo generating listing: $@
	$(NOECHO)$(OBJDUMP) -Mreg-names-raw -d $< | $(CPPFILT) > $@

$(OUTELF).debug.lst: $(OUTELF)
	@echo generating listing: $@
	$(NOECHO)$(OBJDUMP) -Mreg-names-raw -S $< | $(CPPFILT) > $@

$(OUTELF).size: $(OUTELF)
	@echo generating size map: $@
	$(NOECHO)$(NM) -S --size-sort $< > $@

ifeq ($(ENABLE_TRUSTZONE), 1)
$(OUTPUT_TZ_BIN): $(INPUT_TZ_BIN)
	@echo generating TZ output from TZ input
	$(NOECHO)$(OBJCOPY) -I binary -B arm -O elf32-littlearm $(INPUT_TZ_BIN) $(OUTPUT_TZ_BIN)
endif

include arch/$(ARCH)/compile.mk

