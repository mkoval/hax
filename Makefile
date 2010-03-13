RM      = rm -rf

srcdir  = .
VPATH   = $(srcdir)

PROGS   = test skel mikish
ARCHs   = vex_pic cortex

PROG    = skel
ARCH    = cortex

CSOURCE = hax_main.c \
          hax_serial.c
HEADERS = stdint.h \
          hax.h

TARGET  = $(PROG)-$(ARCH).$(ARCHEXT)

include $(ARCH)/Makefile
include $(PROG)/Makefile

OBJECTS  = $(CSOURCE:=.o) $(ASOURCE:=.o)

ALL_CFLAGS = $(ARCH_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(ARCH_LDFLAGS) $(LDFLAGS)
ALL_AFLAGS = $(ARCH_AFLAGS) $(AFLAGS)

.SECONDARY :

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

$(TARGET) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(LD_SCRIPT) $(ALL_LDFLAGS) $(LDOUT_OPT) $@ $^

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< $(CCOUT_OPT)$@

%.asm.o : %.asm $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_AFLAGS) $< /o$@

.PHONY : all clean install rebuild
