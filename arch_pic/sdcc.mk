CC = sdcc
#LD = gplink
AS = gpasm
LD = sdcc

SOURCE += crt0iz_sdcc.c

ARCH_CFLAGS = -mpic16 -p18f8520 
ARCH_CFLAGS += -I$(srcdir) -I$(srcdir)/$(ARCH)
ARCH_CFLAGS += --ivt-loc=0x800 --no-crt
ARCH_CFLAGS += --optimize-cmp --optimize-goto
ARCH_CFLAGS += --optimize-df
#ARCH_CFLAGS += --pstack-model=large

ARCH_ASFLAGS = -p18f8520

OBJECTS     += $(SOURCE:=.o)
#TRASH       += 

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

.SECONDARY:

%.hex : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) $^ $@

%.c.o : %.c
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) -c -o $@ $<

%.asm.o : %.asm
	@echo "AS $(@F)"
	@$(AS) $(ALL_ASFLAGS) -c -o $@ $<

.PHONY: clean
