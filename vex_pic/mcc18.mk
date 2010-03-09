
MCCPATH   = /opt/mcc18
WINPATH   = $(srcdir)/vex_pic/winpath.sh
CC        = '$(MCCPATH)/bin/mcc18'
LD        = '$(MCCPATH)/bin/mplink'
AS        = '$(MCCPATH)/mpasm/mpasm'
IPATH     = '$(MCCPATH)/h'
WIPATH   := '$(shell $(WINPATH) $(IPATH))'
ARCH_CFLAGS = -I=$(WIPATH) -p=18F8520
ARCH_AFLAGS = /p18f8520
LIBPATH   = '$(MCCPATH)/lib'
WLIBPATH := '$(shell $(WINPATH) $(LIBPATH))'
#VEX_LIB   = $(ARCH)/Vex_alltimers.lib
LD_SCRIPT = $(ARCH)/18f8520user.lkr
ARCH_LDFLAGS = $(VEX_LIB) /l $(WLIBPATH) /a INHX32 
LDOUT_OPT = /o
CCOUT_OPT = -fo=

TRASH     = $(TARGET:.hex=.cod) $(TARGET:.hex=.lst) $(OBJECTS:.o=.err)

