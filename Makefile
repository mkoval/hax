TARGET    = $(BUILD_DIR)/vex_fw.hex
MCCPATH   = /opt/mcc18
BUILD_DIR = pic/build
CC        = $(MCCPATH)/bin/mcc18
LD        = $(MCCPATH)/bin/mplink
RM        = rm -f
CFLAGS    = /i "$(MCCPATH)/h" -p=18F8520
LIBPATH   = "$(MCCPATH)/lib/"
VEX_LIB   = pic/Vex_library.lib
LD_SCRIPT = pic/18f8520user.lkr
LDFLAGS   = $(VEX_LIB) /l "$(LIBPATH)" /a INHX32 /o

SOURCE    = hax_main.c \
            pic/hax.c \
            user.c
HEADERS   = hax_int.h \
            hax.h
OBJECTS   = $(patsubst %, $(BUILD_DIR)/%, $(SOURCE:.c=.o))

.SECONDARY :

all : $(TARGET) | $(BUILD_DIR)

install : $(TARGET)
	@vexctl upload $<

clean :
	@echo "CLEAN"
	@-$(RM) $(BUILD_DIR)/*

$(BUILD_DIR) :
	@mkdir $@

%.hex : $(OBJECTS)
	@echo "HEX $(@F)"
	@$(LD) $(LD_SCRIPT) $(LDFLAGS) $(TARGET) $^

$(BUILD_DIR)/%.o : %.c $(HEADERS)
	@echo "OBJ $(@F)"
	@$(CC) $(CFLAGS) $< -fo=$@ -fe=$(@:.o=.err)

.PHONY : all clean install
