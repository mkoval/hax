#include <arch.h>
#include <hax.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#include "exti.h"

/*
 * DIGITAL IO
 */
void digital_setup(index_t index, bool output)
{
	/* Only external digital pins can be used as output. */
	if (IX_DIGITAL(1) <= index && index <= IX_DIGITAL(CT_DIGITAL)) {
		WARN_IX(index);
	} else {
		GPIO_InitTypeDef param;
		param.GPIO_Pin = 1 << ifipin_to_pin[index - 1];
		if (output) {
			param.GPIO_Mode  = GPIO_Mode_IPU;
			param.GPIO_Speed = GPIO_Speed_50MHz;
		} else {
			param.GPIO_Mode  = GPIO_Mode_Out_PP;
		}
		GPIO_Init((GPIO_TypeDef *)ifipin_to_port[index - 1], &param);
	}
}

bool digital_get(index_t index)
{
	if (IX_DIGITAL(1) <= index && index <= IX_DIGITAL(CT_DIGITAL)) {
		GPIO_TypeDef * port = ifipin_to_port[index - OFFSET_DIGITAL - 1];
		index_t         pin = ifipin_to_pin[index - OFFSET_DIGITAL - 1];
		return !!(port->IDR & (1 << pin));
	} else {
		WARN_IX(index);
		return false;
	}
}

void digital_set(index_t index, bool output)
{
	index_t pin = index - OFFSET_DIGITAL;

	// Only external digital pins can be used as output.
	if (index < OFFSET_DIGITAL || index >= OFFSET_DIGITAL + CT_DIGITAL) {
		WARN_IX(index);
	} else if (output) {
		ifipin_to_port[index]->BSRR = 1 << ifipin_to_pin[pin];
	} else {
		ifipin_to_port[index]->BRR  = 1 << ifipin_to_pin[pin];
	}
}
