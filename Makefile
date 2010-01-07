RM        = rm -rf

TARGET    = $(BUILD_DIR)/vex_fw.hex
MCCPATH   = /opt/mcc18
BUILD_DIR = build/pic
CC        = $(MCCPATH)/bin/mcc18
LD        = $(MCCPATH)/bin/mplink
IPATH     = $(MCCPATH)/h
WINIPATH  := $(shell if [ -x `which cygpath` ]; then cygpath -w $(IPATH); elif [ -x `which winepath` ]; then winepath -w $(IPATH); else echo "Not found"; fi )
CFLAGS    = -I="$(WINIPATH)" -p=18F8520
LIBPATH   = $(MCCPATH)/lib/
VEX_LIB   = pic/Vex_alltimers.lib
LD_SCRIPT = pic/18f8520user.lkr
LDFLAGS   = $(VEX_LIB) /l "$(LIBPATH)" /a INHX32 /o

SOURCE    = hax_main.c \
            pic/hax.c \
            pic/pic_init.c\
            user.c
HEADERS   = stdint.h \
            hax.h \
            pic/ifi_lib.h \
            pic/master.h
			
OBJECTS   = $(patsubst %, $(BUILD_DIR)/%, $(SOURCE:.c=.o))

.SECONDARY :

all : $(TARGET) | $(BUILD_DIR)

install : $(TARGET)
	@vexctl upload $<

clean :
	@echo "CLEAN"
	@-$(RM) $(BUILD_DIR)

#This sucks.
$(BUILD_DIR) :
	@mkdir -p $@
	@mkdir -p $@/pic
	@mkdir -p $@/cortex

$(BUILD_DIR)/%.hex : $(OBJECTS) | $(BUILD_DIR)
	@echo "LDHEX $(@F)"
	@$(LD) $(LD_SCRIPT) $(LDFLAGS) $@ $^

$(BUILD_DIR)/%.o : %.c $(HEADERS) | $(BUILD_DIR) 
	@echo "CC $(@F)"
	@$(CC) $(CFLAGS) $< -fo=$@ -fe=$(@:.o=.err)

.PHONY : all clean install
