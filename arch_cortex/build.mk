FWLIB_DIR   = $(ARCH)/lib/fwlib
SRC_STM_LIB = $(ARCH)/lib/startup/startup_stm32f10x_hd.s \
              $(wildcard $(FWLIB_DIR)/src/*.c)
SRC_PRINTF  = $(wildcard $(ARCH)/lib/small_printf/*.c)

SOURCE  += $(ARCH)/hax.c          \
	   $(ARCH)/digital.c      \
	   $(ARCH)/interrupt.c    \
           \
           $(ARCH)/rcc.c          \
           $(ARCH)/spi.c          \
           $(ARCH)/init.c         \
           $(ARCH)/stm32f10x_it.c \
           $(ARCH)/syscall.c      \
           $(ARCH)/usart.c        \
	   $(ARCH)/exti.c         \
           $(SRC_STM_LIB)

ARCH_EXT = bin
include $(ARCH)/gcc.mk
