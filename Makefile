# hax
RM = rm -rf

srcdir = .
VPATH  = $(srcdir)

PROG = mikish
ARCH = vex_pic

SOURCE = hax_main.c hax_serial.c
HEADERS = hax.h

ALL_CFLAGS = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LFLAGS = $(ARCH_LFLAGS) $(LFLAGS)
ALL_AFLAGS = $(ARCH_AFLAGS) $(AFLAGS)

TARGET = $(PROG)-$(ARCH).hex

.PHONY: all
all: $(TARGET)

include $(PROG)/Makefile
include $(ARCH)/Makefile

