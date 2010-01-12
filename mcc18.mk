
MCCPATH   = /opt/mcc18
CC        = $(MCCPATH)/bin/mcc18
LD        = $(MCCPATH)/bin/mplink
IPATH     = $(MCCPATH)/h
WIPATH   := $(shell if [ -x `which cygpath` ]; then cygpath -w $(IPATH); elif [ -x `which winepath` ]; then winepath -w $(IPATH); else echo "Not found"; fi )
CFLAGS    = -I='$(WIPATH)' -p=18F8520
LIBPATH   = $(MCCPATH)/lib
WLIBPATH :=  $(shell if [ -x `which cygpath` ]; then cygpath -w $(LIBPATH); elif [ -x `which winepath` ]; then winepath -w $(LIBPATH); else echo "Path converter not found"; fi )
VEX_LIB   = pic/Vex_alltimers.lib
LD_SCRIPT = pic/18f8520user.lkr
LDFLAGS   = $(VEX_LIB) /l '$(WLIBPATH)' /a INHX32 /o 
