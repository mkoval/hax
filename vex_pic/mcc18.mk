
MCCPATH      = /opt/mcc18
WINPATH      = $(srcdir)/vex_pic/winpath.sh
CC           = '$(MCCPATH)/bin/mcc18'
LD           = '$(MCCPATH)/bin/mplink'
AS           = '$(MCCPATH)/mpasm/mpasm'
IPATH        = '$(MCCPATH)/h'
ICPATH       = '$(ARCH)/include'
WIPATH      := '$(shell $(WINPATH) $(IPATH))'
WICPATH     := '$(shell $(WINPATH) $(ICPATH))'
ARCH_CFLAGS  = -I=$(WICPATH) -I=$(WIPATH) -p=18F8520
ARCH_AFLAGS  = /p18f8520
LIBPATH      = '$(MCCPATH)/lib'
WLIBPATH    := '$(shell $(WINPATH) $(LIBPATH))'
ARCH_LFLAGS = $(ARCH)/18f8520user.lkr /l $(WLIBPATH) /a INHX32 

OBJECTS = $(SOURCE:=.o)
TRASH  += $(TARGET:.hex=.cod) $(TARGET:.hex=.lst) $(OBJECTS:.o=.err)

.SUFFIXES:
.SECONDARY : 

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

$(TARGET) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LFLAGS) $^ /o$@

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< -fo=$@

%.asm.o : %.asm $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_AFLAGS) $< /o$@

.PHONY : clean install rebuild
