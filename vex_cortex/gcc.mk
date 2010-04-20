PREFIX        = arm-none-eabi
CC            = $(PREFIX)-gcc
LD            = $(PREFIX)-gcc
AS            = $(PREFIX)-gcc
MD            = $(PREFIX)-gcc
OBJCOPY       = $(PREFIX)-objcopy
OBJDUMP       = $(PREFIX)objdump
OBJCOPY       = $(REFIX)objcopy
STRIP         = $(PREFIX)strip

# External libraries.
FWLIB_DIR   = $(srcdir)/lib/fwlib
SRC_STM_LIB = $(srcdir)/lib/startup/startup_stm32f10x_hd.s \
              $(wildcard $(FWLIB_DIR)/src/*.c)
SRC_PRINTF  = $(wildcard $(srcdir)/lib/small_printf/*.c)
CC_INC      = -I$(srcdir) -I$(srcdir)/lib/fwlib/inc -I$(srcdir)/lib
LD_INC      = -L$(srcdir)/lib -L$(srcdir)/ld -L$(srcdir)/ld/other

LD_SCRIPT = STM32F103_384K_64K_FLASH.ld
STMPROC   = STM32F10X_HD
HSE_VALUE = 8000000

ARCH_CFLAGS=-MD -D$(STMPROC) -DHSE_VALUE=$(HSE_VALUE) \
           -mthumb -mcpu=cortex-m3 -Wall              \
           -Wno-main -DUSE_STDPERIPH_DRIVER -pipe     \
           -ffunction-sections -fno-unwind-tables     \
           -D_SMALL_PRINTF -DNO_FLOATING_POINT        \
           $(CC_INC) $(CFLAGS)

ARCH_LDFLAGS=$(ALL_CFLAGS)                            \
            -nostartfiles                             \
            -Wl,--gc-sections,-Map=$@.map,-cref       \
            -Wl,-u,Reset_Handler                      \
            -fwhole-program -Wl,-static               \
            $(LD_INC) -T $(LD_SCRIPT)

OBJECTS      += $(SOURCE:=.o)
TRASH        += $(OBJECTS) $(OBJECTS:.o.=.d)          \
                $(shell $(FIND) -E . -regex '.*\.([od]|elf|hex|bin|map|lss|sym|strip)' -delete)

.SECONDARY : 

%.s.o: %.s $(HEADER)
	$(AS) $(ALL_ASFLAGS) -c -o $@ $<

%.c.o: %.c $(HEADER)
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

%.elf: $(OBJ)
	$(LD) $(ALL_LDFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -S -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -S -O binary $< $@

# Create extended listing file from ELF output file.
%.elf.lss: %.elf
	$(OBJDUMP) -h -S $< > $@
# Create a symbol table from ELF output file.
%.elf.sym: %.elf
	$(NM) -n $< > $@

%.elf.strip: %.elf
	$(STRIP) -S $< -o $@
	$(NM) -n $<.strip > $@.sym
	$(OBJDUMP) -h -S $<.strip > $@.lss

.PHONY : all clean install rebuild
