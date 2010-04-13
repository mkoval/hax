PREFIX        = arm-none-eabi
CC            = $(PREFIX)-gcc
LD            = $(PREFIX)-gcc
AS            = $(PREFIX)-gcc
MD            = $(PREFIX)-gcc
OBJCOPY       = $(PREFIX)-objcopy

ARCH_CFLAGS  += -mcpu=cortex-m3 -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_     \
                -mthumb -fsigned-char -ffunction-sections -Wall -Wno-main  \
                -I$(FWLIB_PATH)/inc -I$(srcdir) -D_GPIOG
ARCH_LDFLAGS += $(ARCH_CFLAGS) -Wl,-L$(LDSCRIPT_PATH) -Wl,-T$(LD_SCRIPT)   \
                -Wl,-static,--gc-sections -nostartfiles -Wl,-Map           \
                -l:$(FWLIB_PATH)/lib/STM32F10x_thumb.lib
ARCH_MDFLAGS += $(ARCH_CFLAGS)

TARGET_ELF    = $(TARGET:.hex=.elf)
OBJECTS      += $(SOURCE:=.o)
TRASH        += $(OBJECTS) $(OBJECTS:.o.=.d)

.SECONDARY : 
.SUFFIXES:

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TARGET_ELF) $(TRASH)

$(TARGET_ELF) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) -o $@ $^ $(ALL_LDFLAGS) 

%.hex : %.elf
	@echo "HEX $(@F)"
	@$(OBJCOPY) --target=ihex $< $@ 

-include $(OBJECTS:.o=.d)

%.c.o : %.c 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) -c -o $@ $<

	@$(MD) $(ALL_MDFLAGS) -M -o $<.d $<
	@mv -f $<.d $<.tmp
	@sed -e 's|.*:|$<.o:|' < $<.tmp > $<.d
	@$(RM) $<.tmp

%.s.o : %.s $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) $(ALL_ASFLAGS) -c -o $@ $<

.PHONY : all clean install rebuild
