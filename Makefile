RM      = rm -rf

VPATH   = $(srcdir)

TARGET  = vex_fw.hex

ARCH    = pic

CSOURCE = hax_main.c \
          user.c
HEADERS = stdint.h \
          hax.h

include $(ARCH)/include.mk

OBJECTS   = $(CSOURCE:=.o)

.SECONDARY :

all : $(TARGET)

clean :
	@echo "CLEAN"
	@rm $(OBJECTS) $(TARGET)

%.hex : $(OBJECTS)
	@echo "LDHEX $(@F)"
	@$(LD) $(LD_SCRIPT) $(LDFLAGS) $@ $^

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(CFLAGS) $< -fo=$@ -fe=$(@:.o=.err)

.PHONY : all clean install
