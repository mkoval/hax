RM          = rm -rf

srcdir      = .
VPATH       = $(srcdir)

ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

SOURCE      = hax_main.c
HEADERS     = hax.h

.PHONY: all help pic cortex clean build rebuild
all: build
rebuild : | clean build

include $(PROG)/Makefile
include arch_$(ARCH)/build.mk
TARGET  = $(PROG)-$(ARCH).$(ARCH_EXT)

build: $(TARGET)

help:
	@echo "Valid targets:"
	@echo "  pic"
	@echo "  cortex"

pic:
	@$(MAKE) ARCH="pic" $(MAKEFLAGS) all

cortex:
	@$(MAKE) ARCH="cortex" $(MAKEFLAGS) all

