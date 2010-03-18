MCCPATH      = /opt/mcc18
WINPATH      = $(srcdir)/vex_pic/winpath.sh
CC           = '$(MCCPATH)/bin/mcc18'
LD           = '$(MCCPATH)/bin/mplink'
AS           = '$(MCCPATH)/mpasm/mpasm'
IPATH        = '$(MCCPATH)/h'
WIPATH      := '$(shell $(WINPATH) $(IPATH))'
ARCH_CFLAGS  = -I=$(WIPATH) -p=18F8520
ARCH_AFLAGS  = /p18f8520
LIBPATH      = '$(MCCPATH)/lib'
WLIBPATH    := '$(shell $(WINPATH) $(LIBPATH))'
ARCH_LDFLAGS = $(ARCH)/18f8520user.lkr /l $(WLIBPATH) /a INHX32 

OBJECTS = $(CSOURCE:=.o) $(ASOURCE:=.o)
TRASH  += $(TARGET:.hex=.cod) $(TARGET:.hex=.lst) $(OBJECTS:.o=.err)

.SECONDARY : 

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

$(TARGET) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) $^ /o$@

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< -fo=$@

%.asm.o : %.asm $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_AFLAGS) $< /o$@

.PHONY : all clean install rebuild
