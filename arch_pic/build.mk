SOURCE  += $(ARCH)/hax.c        \
           $(ARCH)/ifi_lib.c    \
           $(ARCH)/ifi_util.asm
HEADERS += $(ARCH)/master.h     \
           $(ARCH)/ifi_lib.h

COMPILER = mcc18
include $(ARCH)/$(COMPILER).mk
