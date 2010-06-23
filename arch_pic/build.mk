SOURCE  += $(ARCH)/hax.c        \
           $(ARCH)/ifi_lib.c
HEADERS += $(ARCH)/master.h     \
           $(ARCH)/ifi_lib.h

ARCH_EXT = hex
COMPILER = sdcc
include $(ARCH)/$(COMPILER).mk
