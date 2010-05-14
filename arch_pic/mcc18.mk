CC           = '$(MCCPATH)/bin/mcc18'
LD           = '$(MCCPATH)/bin/mplink'
AS           = '$(MCCPATH)/mpasm/mpasm'
MD           = gcc

MCCPATH      = /opt/mcc18
WINPATH      = $(srcdir)/vex_pic/winpath.sh
IPATH        = '$(MCCPATH)/h'
ICPATH       = '$(srcdir)/$(ARCH)/include'
IAPATH       = '$(srcdir)/$(ARCH)'
WIPATH      := '$(shell $(WINPATH) $(IPATH))'
WICPATH     := '$(shell $(WINPATH) $(ICPATH))'
WIAPATH     := '$(shell $(WINPATH) $(IAPATH))'
LIBPATH      = '$(MCCPATH)/lib'
WLIBPATH    := '$(shell $(WINPATH) $(LIBPATH))'

ARCH_CFLAGS  = -I=$(WICPATH) -I=$(WIPATH) -I=$(srcdir) -I=$(WIAPATH)   \
               -p=18F8520 /DARCH_PIC
ARCH_ASFLAGS = /p18f8520
ARCH_LDFLAGS = $(ARCH)/18f8520user.lkr /l $(WLIBPATH) /a INHX32

OBJECTS     += $(SOURCE:=.o)
TRASH       += $(TARGET:.hex=.cod) \
               $(TARGET:.hex=.lst) \
               $(OBJECTS:.o=.err)  \
               $(OBJECTS:.o=.d)

.SUFFIXES:
.SECONDARY:

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

$(TARGET) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) $^ /o$@

-include $(OBJECTS:.o=.d)

%.c.o : %.c
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< -fo=$@

%.asm.o : %.asm
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_ASFLAGS) $< /o$@

.PHONY : clean install rebuild
