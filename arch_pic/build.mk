SOURCE  += $(ARCH)/hax.c        \
           $(ARCH)/ifi_lib.c    \
           $(ARCH)/ifi_util.asm
HEADERS += $(ARCH)/master.h     \
           $(ARCH)/ifi_lib.h

ARCH_EXT = hex
COMPILER = sdcc
include $(ARCH)/$(COMPILER).mk
