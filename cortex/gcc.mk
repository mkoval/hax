
CC           = 'arm-none-eabi-gcc'
LD           = 'arm-none-eabi-gcc'
AS           = 'arm-none-eabi-gcc'
IPATH        = '/usr/local/stm32/arm-none-eabi/include/'
ARCH_CFLAGS  = -MD -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_ -mthumb              \
               -mcpu=cortex-m3 -fsigned-char -ffunction-sections -Wall         \
               -mlittle-endian -c
ARCH_AFLAGS  =
ARCH_LDFLAGS = -mcpu=cortex-m3 -mthumb -Wl,-T -Xlinker -u _start -Wl,-static   \
               -Wl,--gc-sections -nostartfiles -Wl,-Map -Xlinker
TRASH        =

