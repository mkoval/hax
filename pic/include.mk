include mcc18.mk

CSOURCE += $(ARCH)/hax.c \
	   $(ARCH)/pic_init.c

HEADERS += $(ARCH)/master.h \
	   $(ARCH)/ifi_lib.h
