CC           = '$(MCCPATH)/bin/mcc18'
LD           = '$(MCCPATH)/bin/mplink'
AS           = '$(MCCPATH)/mpasm/mpasm'
MD           = gcc

MCCPATH      = /opt/mcc18
WINPATH      = $(srcdir)/vex_pic/winpath.sh
IPATH        = '$(MCCPATH)/h'
ICPATH       = '$(ARCH)/include'
WIPATH      := '$(shell $(WINPATH) $(IPATH))'
WICPATH     := '$(shell $(WINPATH) $(ICPATH))'
LIBPATH      = '$(MCCPATH)/lib'
WLIBPATH    := '$(shell $(WINPATH) $(LIBPATH))'

ARCH_CFLAGS  = -I=$(WICPATH) -I=$(WIPATH) -I=$(srcdir) -p=18F8520
ARCH_ASFLAGS = /p18f8520
ARCH_LDFLAGS = $(ARCH)/18f8520user.lkr /l $(WLIBPATH) /a INHX32
ARCH_MDFLAGS = -MM -DMCC18_24 -D__18CXX -DUNCHANGEABLE_DEFINITION_AREA \
               -L$(srcdir)

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

    # Generate auto-dependancy files using GCC.
	@$(MD) $(ALL_MDFLAGS) $< -o $<.d
	@mv -f $<.d $<.tmp
	@sed -e 's|.*:|$<.o:|' < $<.tmp > $<.d
	@$(RM) $<.tmp

%.asm.o : %.asm
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_ASFLAGS) $< /o$@

.PHONY : clean install rebuild
