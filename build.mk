ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

# Use the path to this Makefile as the base directory of HAX.
srcdir     := $(dir $(lastword $(MAKEFILE_LIST)))
srcdir     := $(srcdir:/=)
ARCH        = $(srcdir)/arch_$(arch)
TARGET      = $(prog)-$(arch).$(ARCH_EXT)

ifneq ($(arch),cortex)
ifneq ($(arch),pic)
$(error "unsupported architecture")
endif
endif

ifndef prog
$(error "unnamed program")
endif

ifeq ($(serial),)
$(warning "no serial port specified; 'make install' may fail")
endif


.SUFFIXES:
.PHONY: all clean build rebuild install

all: build

rebuild: | clean build

-include $(SOURCE:=.d)
include $(ARCH)/build.mk

build: $(TARGET)

mrproper:
	@echo "MRPROPER"
	@$(FIND) . -print \
	| $(GREP) '.*\.\([od]\|elf\|hex\|bin\|map\|lss\|sym\|strip\)$$' \
	| $(XARGS) -- $(RM)

ifeq ($(arch),cortex)
install: $(TARGET)
	@echo "UPLOAD $^"
	@$(srcdir)/arch_cortex/jtag/stm32loader.py -ewv -p"$(serial)" -b115200 $(TARGET)
endif
