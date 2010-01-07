RM        = rm -rf

include mcc18.mk

TARGET    = $(BUILD_DIR)/vex_fw.hex

BUILD_DIR = build/pic

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
	@$(RM) $(BUILD_DIR)

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
