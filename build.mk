ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

# Use the path to this Makefile as the base directory of HAX.
LIBDIR     := $(dir $(lastword $(MAKEFILE_LIST)))
LIBDIR     := $(LIBDIR:/=)
ARCHDIR     = $(LIBDIR)/arch_$(arch)
TARGET      = $(prog)-$(arch).$(ARCH_EXT)

ifneq ($(arch),cortex)
ifneq ($(arch),pic)
$(error "unsupported architecture")
endif
endif

ifndef prog
$(error "unnamed program")
endif

VER:=$(shell $(LIBDIR)/get_version)


.SUFFIXES:
.PHONY: all clean build rebuild install

all: build

rebuild: | clean build

-include $(SOURCE:=.d)
include $(ARCHDIR)/build.mk

build: $(TARGET)

.PHONY: mrproper
mrproper:
	@echo "MRPROPER"
	@$(FIND) . -print \
	| $(GREP) '.*\.\([od]\|elf\|hex\|bin\|map\|lss\|sym\|strip\)$$' \
	| $(XARGS) -- $(RM)

