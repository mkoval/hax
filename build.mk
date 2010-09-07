ALL_CFLAGS  = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_ASFLAGS = $(ARCH_ASFLAGS) $(ASFLAGS)
ALL_MDFLAGS = $(ARCH_MDFLAGS) $(MDFLAGS)

ARCH   = $(HAX_PATH)/arch_$(arch)
TARGET = $(prog)-$(arch).$(ARCH_EXT)
srcdir = $(HAX_PATH)

.SUFFIXES:
.PHONY: all clean build rebuild

all: build

rebuild : | clean build

include $(ARCH)/build.mk

build: $(TARGET)

mrproper:
	@echo "MRPROPER"
	@$(FIND) . -print \
	| $(GREP) '.*\.\([od]\|elf\|hex\|bin\|map\|lss\|sym\|strip\)$$' \
	| $(XARGS) -- $(RM)

