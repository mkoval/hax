
MCCPATH   = /opt/mcc18
WINPATH   = $(srcdir)/vex_pic/winpath.sh
CC        = '$(MCCPATH)/bin/mcc18'
LD        = '$(MCCPATH)/bin/mplink'
IPATH     = '$(MCCPATH)/h'
WIPATH   := '$(shell $(WINPATH) $(IPATH))'
CFLAGS    = -I=$(WIPATH) -p=18F8520
LIBPATH   = '$(MCCPATH)/lib'
WLIBPATH := '$(shell $(WINPATH) $(LIBPATH))'
VEX_LIB   = $(ARCH)/Vex_alltimers.lib
LD_SCRIPT = $(ARCH)/18f8520user.lkr
LDFLAGS   = $(VEX_LIB) /l $(WLIBPATH) /a INHX32 /o 

TRASH     = $(TARGET:.hex=.cod) $(TARGET:.hex=.lst) $(OBJECTS:.o=.err)