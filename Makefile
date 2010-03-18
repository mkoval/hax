RM = rm -rf

srcdir = .
VPATH  = $(srcdir)

PROG = skel
ARCH = vex_pic

CSOURCE = hax_main.c hax_serial.c
HEADERS = stdint.h hax.h

include $(PROG)/Makefile
include $(ARCH)/Makefile

ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_AFLAGS  = $(ARCH_AFLAGS) $(AFLAGS)

