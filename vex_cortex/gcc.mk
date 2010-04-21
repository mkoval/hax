ARCH_PREFIX   = arm-none-eabi-
CC            = $(ARCH_PREFIX)gcc
LD            = $(ARCH_PREFIX)gcc
AS            = $(ARCH_PREFIX)gcc
MD            = $(ARCH_PREFIX)gcc
OBJCOPY       = $(ARCH_PREFIX)objcopy
OBJDUMP       = $(ARCH_PREFIX)objdump
STRIP         = $(ARCH_PREFIX)strip
FIND          = find

# External libraries.
CC_INC      = -I$(srcdir) -I$(ARCH)/lib/fwlib/inc -I$(ARCH)/lib
LD_INC      = -L$(ARCH)/lib -L$(ARCH)/ld -L$(ARCH)/ld/other

LD_SCRIPT = STM32F103_384K_64K_FLASH.ld
STMPROC   = STM32F10X_HD
HSE_VALUE = 8000000

ARCH_CFLAGS=-MD -D$(STMPROC) -DHSE_VALUE=$(HSE_VALUE) \
           -mthumb -mcpu=cortex-m3 -Wall              \
           -Wno-main -DUSE_STDPERIPH_DRIVER -pipe     \
           -ffunction-sections -fno-unwind-tables     \
           -D_SMALL_PRINTF -DNO_FLOATING_POINT        \
           -DARCH_CORTEX $(CC_INC) $(CFLAGS)

ARCH_LDFLAGS=$(ALL_CFLAGS)                            \
            -nostartfiles                             \
            -Wl,--gc-sections,-Map=$@.map,-cref       \
            -Wl,-u,Reset_Handler                      \
            -fwhole-program -Wl,-static               \
            $(LD_INC) -T $(LD_SCRIPT)

OBJECTS       = $(SOURCE:=.o)
TRASH        += $(TARGET) $(OBJECTS) $(OBJECTS:.o.=.d)


clean :
	@$(RM) $(TRASH)
	@$(FIND) -E . -regex '.*\.([od]|elf|hex|bin|map|lss|sym|strip)' -delete
	@echo "CLEAN"

rebuild : clean all

.SECONDARY : 

%.s.o: %.s $(HEADER)
	@$(AS) $(ALL_ASFLAGS) -c -o $@ $<
	@echo "AS $<"

%.c.o: %.c $(HEADER)
	@$(CC) $(ALL_CFLAGS) -c -o $@ $<
	@echo "CC $<"

%.elf: $(OBJECTS)
	@$(LD) $(ALL_LDFLAGS) -o $@ $^
	@echo "LD $<"

%.hex: %.elf
	@$(OBJCOPY) -S -O ihex $< $@
	@echo "HEX $<"

%.bin: %.elf
	@$(OBJCOPY) -S -O binary $< $@
	@echo "BIN $<"

# Create extended listing file from ELF output file.
%.elf.lss: %.elf
	@$(OBJDUMP) -h -S $< > $@
	@echo "LSS $<"

# Create a symbol table from ELF output file.
%.elf.sym: %.elf
	@$(NM) -n $< > $@
	@echo "SYM $<"

.PHONY : all clean install rebuild
