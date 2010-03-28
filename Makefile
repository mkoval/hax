RM          = rm -rf

srcdir      = .
VPATH       = $(srcdir)

ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

SOURCE      = hax_main.c hax_serial.c
HEADERS     = hax.h
TARGET      = $(PROG)-$(ARCH).hex

.PHONY: all

all: $(TARGET)

include $(PROG)/Makefile
include $(ARCH)/Makefile
