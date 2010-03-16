STARTUP("Startup.o")

INPUT("easyCRuntimeLib.lib")

EXTERN( __io_putchar )
EXTERN( _write )

INPUT("STM32x_io_putchar_thumb.a")
INPUT("e_stdio_thumb.a")

OUTPUT("easyCRuntime.elf")

INCLUDE "cortex/vexlib/Scripts/STM32F103_384K_64K_FLASH.ld"
