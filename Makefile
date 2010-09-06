RM          = rm -rf

srcdir      = .
VPATH       = $(srcdir)

ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

SOURCE      = 
HEADERS     = hax.h arch_check.h prog_check.h

.SUFFIXES:
.PHONY: all help arch_pic arch_cortex clean build rebuild
all: build
rebuild : | clean build

ARCH = arch_$(arch)
PROG = $(prog)
include $(PROG)/Makefile
include $(ARCH)/build.mk
TARGET  = $(PROG)-$(ARCH).$(ARCH_EXT)

build: $(TARGET)

