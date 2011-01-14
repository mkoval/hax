#include <hax.h>
#include <stdint.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_misc.h"

/* PD0(10),PD1(11),2(X),3(X),4(X),5(X),
 * PC6(2),PC7(3),PE8(6),PE9(0),PE10(7),
 * PE11(1),PE12(8),PE13(4),PE14(5)
 */
/* Not used. (replace PC7(3))
 * PE7(9)
 */

/*     PE9, PE11,  PC6,  PC7, PE13, PE14,  PE8, PE10, PE12,  PE7,  PD0,  PD1*/
static GPIO_TypeDef *const ifipin_to_port[12] =
    {GPIOE,GPIOE,GPIOC,GPIOC,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOD,GPIOD};
static const int8_t ifipin_to_pin[12] =
    {    9,   11,    6,    7,   13,   14,    8,   10,   12,    7,    0,    1};

static const uint8_t pin_to_ifipin [16] =
	/* 0   1    2    3    4    5  6    7 */
	{ 10, 11, 255, 255, 255, 255, 2,   3,
	/* 8   9   10   11   12   13 14   15 */
	   6,  0,   7,   1,   8,   4, 5, 255};

static isr_t isr_callback[12];

/* HAX interrupt code */
void interrupt_reg_isr(index_t index, isr_t isr) {
	index_t pin = index - IX_DIGITAL(1);
	if (!(0 <= pin && pin <= 11 && pin != 9)) {
		WARN_IX(index);
		return;
	}
	isr_callback[pin] = isr;
}

void interrupt_setup(index_t index, isr_t isr)
{
	index_t pin = index - IX_DIGITAL(1);
	if (!(0 <= pin && pin <= 11 && pin != 9)) {
		WARN_IX(index);
		return;
	}

	isr_callback[index] = isr;
	EXTI->EMR   &= ~(1 << pin); /* disable event request */
	EXTI->RTSR  |=  (1 << pin); /* enable rising edge trigger */
	EXTI->FTSR  |=  (1 << pin); /* enable falling edge trigger */
}

void interrupt_set(index_t index, bool enable)
{
	index_t pin = index - IX_DIGITAL(1);
	if (!(0 <= pin && pin <= 11 && pin != 9)) {
		WARN_IX(index);
		return;
	}

	if (enable) {
		digital_setup(ifipin_to_pin[pin], false);
		EXTI->IMR  |=  (1 << pin); /* enable interrupt */
	} else {
		EXTI->IMR  &= ~(1 << pin); /* disable interrupt */
	}
}

/* HAX digital io api */
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


/* interrupt implimentation */
#define __isr __attribute__((interrupt))

#define CALL_ISR(_i_)                                          \
	if (EXTI->PR & (1<<(_i_))) {                           \
		EXTI->PR = 1<<(_i_);                           \
		uint8_t ri = pin_to_ifipin[_i_];	       \
		if (isr_callback[ri]) {                        \
			isr_callback[ri](digital_get(ri));     \
		}                                              \
	}

__isr void EXTI0_IRQHandler(void) {
	CALL_ISR(0);
}

__isr void EXTI1_IRQHandler(void) {
	CALL_ISR(1);
}

__isr void EXTI9_5_IRQHandler(void) {
#if 0
	CALL_ISR(5); // not connected
#endif
	CALL_ISR(6);
	CALL_ISR(7);
	CALL_ISR(8);
	CALL_ISR(9);
}

__isr void EXTI15_10_IRQHandler(void) {
	CALL_ISR(10);
	CALL_ISR(11);
	CALL_ISR(12);
	CALL_ISR(13);
	CALL_ISR(14);
}

void exti_init(void) {
	NVIC_InitTypeDef NVIC_param;

	/* PE9(0),PE11(1),PC6(2),PC7(3),PE13(4),PE14(5),
	 * PE8(6),PE10(7),PE12(8),PE7(9),PD0(10),PD1(11)
	 */
	AFIO->EXTICR[0] =
		( AFIO_EXTICR1_EXTI0_PD // PD0 (10)
		| AFIO_EXTICR1_EXTI1_PD // PD1 (11)
		/* | AFIO_EXTCR1_EXTI2_Px */
		/* | AFIO_EXTCR1_EXTI3_Px */
		);

	/* Note: PE7(9) is not connected to
	 * EXTI interrupt due to hardware
	 * design limitations
	 */
	AFIO->EXTICR[1] =
		( /* AFIO_EXTICR2_EXTI4_Px */
		/* | AFIO_EXTICR2_EXTI5_Px */
		  AFIO_EXTICR2_EXTI6_PC // PC6 (2)
		| AFIO_EXTICR2_EXTI7_PC // PC7 (3)
		);

	AFIO->EXTICR[2] =
		( AFIO_EXTICR3_EXTI8_PE // PE8 (6)
		| AFIO_EXTICR3_EXTI9_PE // PE9 (0)
		| AFIO_EXTICR3_EXTI10_PE // PE10 (7)
		| AFIO_EXTICR3_EXTI11_PE // PE11 (1)
		);

	AFIO->EXTICR[3] =
		( AFIO_EXTICR4_EXTI12_PE // PE12 (8)
		| AFIO_EXTICR4_EXTI13_PE // PE13 (4)
		| AFIO_EXTICR4_EXTI14_PE // PE14 (5)
		/*| AFIO_EXTICR4_EXTI15_Px */
		);

	EXTI->IMR =
		( EXTI_IMR_MR0
		| EXTI_IMR_MR1
		| EXTI_IMR_MR6
		| EXTI_IMR_MR7
		| EXTI_IMR_MR8
		| EXTI_IMR_MR9
		| EXTI_IMR_MR10
		| EXTI_IMR_MR11
		| EXTI_IMR_MR12
		| EXTI_IMR_MR13
		| EXTI_IMR_MR14
		);

	/* Enable the EXTI0 Interrupt */
	NVIC_param.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_param.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_param.NVIC_IRQChannelSubPriority = 2;
	NVIC_param.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_param);

	/* Enable the EXTI1 Interrupt */
	NVIC_param.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_param.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_param.NVIC_IRQChannelSubPriority = 2;
	NVIC_param.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_param);

	/* Enable the EXTI9_5 Interrupt */
	NVIC_param.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_param.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_param.NVIC_IRQChannelSubPriority = 2;
	NVIC_param.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_param);

	/* Enable the EXTI15_10 Interrupt */
	NVIC_param.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_param.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_param.NVIC_IRQChannelSubPriority = 2;
	NVIC_param.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_param);
}
