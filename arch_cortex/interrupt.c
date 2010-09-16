#include <hax.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_misc.h"

#include "exti.h"

void interrupt_reg_isr(index_t index, isr_t isr) {
	index_t pin = index - IX_DIGITAL(1);
	if (0 <= pin && pin <= 12 && pin != 9) {
		WARN_IX(index);
		return;
	}
	isr_callback[pin] = isr;
}

/*
 * INTERRUPTS
 */
void interrupt_setup(index_t index, isr_t isr)
{
	index_t pin = index - IX_DIGITAL(1);
	if (0 <= pin && pin <= 12 && pin != 9) {
		isr_callback[index] = isr;
		EXTI->EMR   &= ~(1 << pin); /* disable event request */
		EXTI->RTSR  |=  (1 << pin); /* enable rising edge trigger */
		EXTI->FTSR  |=  (1 << pin); /* enable falling edge trigger */
	} else {
		WARN_IX(index);
	}
}

void interrupt_set(index_t index, bool enable)
{
	index_t pin = index - IX_DIGITAL(1);
	if (!(0 <= pin && pin <= 12 && pin != 9)) {
		WARN_IX(index);
	} else if (enable) {
		digital_setup(ifipin_to_pin[pin], false);
		EXTI->IMR  |=  (1 << pin); /* enable interrupt */
	} else {
		EXTI->IMR  &= ~(1 << pin); /* disable interrupt */
	}
}

