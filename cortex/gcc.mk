CC           = 'arm-none-eabi-gcc'
LD           = 'arm-none-eabi-gcc'
AS           = 'arm-none-eabi-gcc'
CCOUT_OPT    = -o 
ARCH_CFLAGS  = -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_ -mthumb -mcpu=cortex-m3  \
               -fsigned-char -ffunction-sections -Wall -mlittle-endian         \
               -Wno-main -c
ARCH_AFLAGS  =
ARCH_LDFLAGS = -mcpu=cortex-m3 -mthumb -Wl,-T -Xlinker -u _start -Wl,-static   \
               -Wl,--gc-sections -nostartfiles -Wl,-Map -Xlinker
TRASH        = $(CSOURCE:.c=.c.d)
