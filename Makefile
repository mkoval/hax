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

.PHONY: all help vex_pic vex_cortex easyc_cortex clean


all: $(TARGET)

help:
	@echo "Valid targets:"
	@echo "  vex_pic"
	@echo "  vex_cortex"
	@echo "  easyc_cortex"

vex_pic:
	@$(MAKE) ARCH="vex_pic" $(MAKEFLAGS) all

vex_cortex:
	@$(MAKE) ARCH="vex_cortex" $(MAKEFLAGS) all

easyc_cortex:
	@$(MAKE) ARCH="easyc_cortex" $(MAKEFLAGS) all


include $(PROG)/Makefile
include $(ARCH)/Makefile
# DO NOT DELETE

hax.o: compilers.h /usr/include/stdint.h /usr/include/stdbool.h
