SOURCE  += $(ARCHDIR)/hax.c        \
           $(ARCHDIR)/ifi_lib.c
HEADERS += $(ARCHDIR)/master.h     \
           $(ARCHDIR)/ifi_lib.h

ARCH_EXT = hex
COMPILER = sdcc
include $(ARCHDIR)/$(COMPILER).mk
