CC           = 'arm-none-eabi-gcc'
LD           = 'arm-none-eabi-gcc'
AS           = 'arm-none-eabi-as'
CCOUT_OPT    = -o 
LDOUT_OPT    = -o 
ARCH_CFLAGS  = -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_ -mthumb -mcpu=cortex-m3  \
               -fsigned-char -ffunction-sections -Wall -mlittle-endian         \
               -Wno-main -c
ARCH_AFLAGS  =
ARCH_LDFLAGS = -Wl,-T -Xlinker 'cortex/vexlib/Scripts/easyCRuntimeInit.elf.ld' \
               -L'cortex/vexlib/Startup' -L'cortex/vexlib/easyCRuntime'        \
               -L'cortex/vexlib/Link' -mpu=cortex-m3 -mthumb -u _start         \
               -Wl,-static -Wl,--gc-sections -nostartfiles
TRASH        = $(CSOURCE:.c=.c.d)
