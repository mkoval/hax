PREFIX        = arm-none-eabi
CC            = $(PREFIX)-gcc
LD            = $(PREFIX)-gcc
AS            = $(PREFIX)-gcc
OBJCOPY       = $(PREFIX)-objcopy

ARCH_CFLAGS  += -mcpu=cortex-m3 -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_      \
                -mthumb -fsigned-char -ffunction-sections -Wall -Wno-main   \
		-I$(srcdir)
ARCH_LDFLAGS += $(ARCH_CFLAGS) -Wl,-L$(LDSCRIPT_PATH) -Wl,-T$(LD_SCRIPT)    \
                $(STARTUP_OBJ) $(STARTUPI_OBJ) -u _start                    \
                -Wl,-static,--gc-sections -nostartfiles            \

TARGET_ELF    = $(TARGET:.hex=.elf)
OBJECTS      += $(SOURCE:=.o)
TRASH        += $(OBJECTS)

.SECONDARY : 
.SUFFIXES:

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TARGET_ELF) $(TRASH)

$(TARGET_ELF) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) $^ -o $@

%.hex : %.elf
	@echo "HEX $(@F)"
	@$(OBJCOPY) --target=ihex $< $@ 

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) -c $< -o $@ 

%.s.o : %.s $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) $(ALL_AFLAGS) -c $< -o $@

.PHONY : all clean install rebuild
