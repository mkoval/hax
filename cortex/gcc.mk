CC      = 'arm-none-eabi-gcc'
LD      = 'arm-none-eabi-gcc'
AS      = 'arm-none-eabi-as'
OBJCOPY = 'arm-none-eabi-objcopy'

ARCH_CFLAGS  = -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_ -mthumb -mcpu=cortex-m3  \
               -fsigned-char -ffunction-sections -Wall -mlittle-endian         \
               -Wno-main -c
ARCH_LDFLAGS = -Wl,-T -Xlinker 'cortex/vexlib/Scripts/easyCRuntimeInit.elf.ld' \
               -L'cortex/vexlib/Startup' -L'cortex/vexlib/easyCRuntime'        \
               -L'cortex/vexlib/Link' -mpu=cortex-m3 -mthumb -u _start         \
               -Wl,-static -Wl,--gc-sections -nostartfiles

OBJECTS = $(CSOURCE:.c=.c.o)
TRASH   = $(OBJECTS)

.SECONDARY : 

all : $(TARGET)

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TARGET_ELF) $(TRASH)

$(TARGET_ELF) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) $^ -o $@

%.hex : %.elf
	@echo "HEX $(@F)"
	@$(OBJCOPY) $< --target=ihex $@

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< -o $@

%.asm.o : %.asm $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_AFLAGS) $< -o $@

.PHONY : all clean install rebuild
