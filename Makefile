RM      = rm -rf

srcdir  = .
VPATH   = $(srcdir)

PROGS   = test skel mikish
ARCHs   = vex_pic cortex

PROG    = mikish
ARCH    = vex_pic

CSOURCE = hax_main.c \
          hax_serial.c
HEADERS = stdint.h \
          hax.h

TARGET  = $(PROG)-$(ARCH).$(ARCHEXT)

include $(ARCH)/Makefile
include $(PROG)/Makefile

OBJECTS  = $(CSOURCE:=.o)

ALL_CFLAGS = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)

.SECONDARY :

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

%.hex : $(OBJECTS)
	@echo "LDHEX $(@F)"
	@$(LD) $(LD_SCRIPT) $(ALL_LDFLAGS) $(LDOUT_OPT)$@ $^

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< $(CCOUT_OPT)$@ -fe=$(@:.o=.err)

.PHONY : all clean install rebuild
