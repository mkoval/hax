RM        = rm -rf

include mcc18.mk

TARGET    = vex_fw.hex


SOURCE    = hax_main.c \
            pic/hax.c \
            pic/pic_init.c\
            user.c
HEADERS   = stdint.h \
            hax.h \
            pic/ifi_lib.h \
            pic/master.h
			
OBJECTS   = $(SOURCE:.c=.o)

.SECONDARY :

all : $(TARGET)

clean :
	@echo "CLEAN"
	@rm $(OBJECTS) $(TARGET)

%.hex : $(OBJECTS)
	@echo "LDHEX $(@F)"
	@$(LD) $(LD_SCRIPT) $(LDFLAGS) $@ $^

%.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(CFLAGS) $< -fo=$@ -fe=$(@:.o=.err)

.PHONY : all clean install
