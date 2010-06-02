RM          = rm -rf

srcdir      = .
VPATH       = $(srcdir)

ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

SOURCE      = hax_main.c
HEADERS     = hax.h

all: build

include $(ARCH)/build.mk
TARGET  = $(PROG)-$(ARCH).$(ARCH_EXT)


.PHONY: all help vex_pic vex_cortex easyc_cortex clean build

build: $(TARGET)

help:
	@echo "Valid targets:"
	@echo "  arch_pic"
	@echo "  arch_cortex"

vex_pic:
	@$(MAKE) ARCH="arch_pic" $(MAKEFLAGS) all

vex_cortex:
	@$(MAKE) ARCH="arch_cortex" $(MAKEFLAGS) all

include $(PROG)/Makefile
