FWLIB_DIR   = $(ARCHDIR)/lib/fwlib
SRC_STM_LIB = $(ARCHDIR)/lib/startup/startup_stm32f10x_hd.s \
              $(wildcard $(FWLIB_DIR)/src/*.c)
SRC_PRINTF  = $(wildcard $(ARCHDIR)/lib/small_printf/*.c)

SOURCE  += $(ARCHDIR)/hax.c          \
           $(ARCHDIR)/clocks.c       \
           $(ARCHDIR)/spi.c          \
           $(ARCHDIR)/init.c         \
           $(ARCHDIR)/stm32f10x_it.c \
           $(ARCHDIR)/syscall.c      \
           $(ARCHDIR)/usart.c        \
           $(ARCHDIR)/exti.c         \
           $(ARCHDIR)/motor.c        \
           $(SRC_STM_LIB)
ARCH_EXT = bin

install: $(TARGET)
	@echo "UPLOAD $^"
	@$(LIBDIR)/arch_cortex/jtag/stm32loader.py -ewv -p"$(serial)" -b115200 $(TARGET)

include $(ARCHDIR)/gcc.mk
