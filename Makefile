RM      = rm -rf

srcdir  = .
VPATH   = $(srcdir)

PROGS   = test skel
ARCHs   = vex_pic cortex

PROG    = test
ARCH    = vex_pic

CSOURCE = hax_main.c
HEADERS = stdint.h \
          hax.h

include $(ARCH)/Makefile
include $(PROG)/Makefile

OBJECTS   = $(CSOURCE:=.o)

.SECONDARY :

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TRASH)

%.hex : $(OBJECTS)
	@echo "LDHEX $(@F)"
	@$(LD) $(LD_SCRIPT) $(LDFLAGS) $@ $^

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(CFLAGS) $< -fo=$@ -fe=$(@:.o=.err)

.PHONY : all clean install rebuild
