CC = sdcc
#LD = gplink
AS = gpasm
LD = $(CC)

SOURCE += $(srcdir)/$(ARCH)/crt0iz_sdcc.c
SOURCE += $(srcdir)/$(ARCH)/spi_open.c
SOURCE += $(srcdir)/$(ARCH)/ifi_util_sdcc.asm

ARCH_CFLAGS = -mpic16 -p18f8520
ARCH_CFLAGS += -I$(srcdir) -I$(srcdir)/$(ARCH)
ARCH_CFLAGS += -I$(srcdir)/$(ARCH)/h
ARCH_CFLAGS += --ivt-loc=0x800 --no-crt
ARCH_CFLAGS += --optimize-cmp
ARCH_CFLAGS += --optimize-df
#ARCH_CFLAGS += --pstack-model=large

ARCH_LDFLAGS = -Wl,-s,$(srcdir)/$(ARCH)/18f8520user_sdcc.lkr $(ARCH_CFLAGS)
ARCH_ASFLAGS = -p18f8520

OBJECTS = $(SOURCE:=.o)
#TRASH       += 

.PHONY: clean
clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

.SECONDARY:

%.c.o : %.c
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) -c -o $@ $<

%.asm.o : %.asm
	@echo "AS $(@F)"
	@$(AS) $(ALL_ASFLAGS) -c -o $@ $<

%.hex : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) -o $@ $^
