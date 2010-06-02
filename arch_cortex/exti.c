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
static const uint8_t pin_to_ifipin [16] =
	/* 0   1    2    3    4    5  6    7 */   
	{ 10, 11, 255, 255, 255, 255, 2,   3,
	/* 8   9   10   11   12   13 14   15 */
	   6,  0,   7,   1,   8,   4, 5, 255};

static isr_t isr_callback[12];

#define IS_DIGITAL(_x_)   ( 1 <= (_x_) && (_x_) <= 20)
#define IS_ANALOG(_x_)    (13 <= (_x_) && (_x_) <= 20)
#define IS_INTERRUPT(_x_) ( 1 <= (_x_) && (_x_) <= 12 && (_x_) != 9)

#define __isr __attribute__((interrupt))

#define CALL_ISR(_i_)                                          \
        if (EXTI->PR & (1<<(_i_))) {                           \
                EXTI->PR = 1<<(_i_);                           \
                uint8_t ri = pin_to_ifipin[_i_];	       \
                if (isr_callback[ri]) {                    \
                        isr_callback[ri](digital_get(ri)); \
                }                                          \
        }

__isr void EXTI0_IRQHandler(void) {
	CALL_ISR(0);
}

__isr void EXTI1_IRQHandler(void) {
	CALL_ISR(1);
}

__isr void EXTI9_5_IRQHandler(void) {
	// CALL_ISR(5); // not connected
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
	CALL_ISR(15);
}


    /* PE9, PE11,  PC6,  PC7, PE13, PE14,  PE8, PE10, PE12,  PE7,  PD0,  PD1*/
static GPIO_TypeDef *const ifipin_to_port[12] = 
    {GPIOE,GPIOE,GPIOC,GPIOC,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOD,GPIOD};

static const int8_t ifipin_to_pin[12] =
    {    9,   11,    6,    7,   13,   14,    8,   10,   12,    7,    0,    1};

bool digital_get(index_t index) {
	/* TODO Enable support for using analog pins as digital IOs. */
	if (!IS_DIGITAL(index) || IS_ANALOG(index)) {
		ERROR();
		return false;
	}

	GPIO_TypeDef *port = ifipin_to_port[index - 1];
	index_t       pin  = ifipin_to_pin[index - 1];
	return (port->IDR & ( 1 << pin )) == (1 << pin);
}

void digital_set(index_t index, bool pull_high) {
	/* TODO Enable support for using analog pins as digital IOs. */
	if (!IS_DIGITAL(index) || IS_ANALOG(index)) {
		ERROR();
		return;
	}

	if (pull_high) {
		ifipin_to_port[index - 1]->BSRR = 1 << ifipin_to_pin[index - 1];
	} else {
		ifipin_to_port[index - 1]->BRR  = 1 << ifipin_to_pin[index - 1];
	}
}

void pin_set_io(index_t index, bool set_output ) {	
	GPIO_InitTypeDef GPIO_param;

	/* TODO Enable support for using analog pins as digital IOs. */
	if (!IS_DIGITAL(index) || IS_ANALOG(index)) {
		ERROR();
		return;
	}
	
	GPIO_param.GPIO_Pin = (uint16_t)(1 << ifipin_to_pin[index - 1]);
	
	if (!set_output) {
		GPIO_param.GPIO_Mode = GPIO_Mode_IPU;
	} else {
		GPIO_param.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_param.GPIO_Mode = GPIO_Mode_Out_PP;
	}
	
	GPIO_Init((GPIO_TypeDef *)ifipin_to_port[index - 1], &GPIO_param);
}

void interrupt_reg_isr(index_t index, isr_t isr) {
	if (!IS_INTERRUPT(index)) {
		ERROR();
		return;
	}

	isr_callback[index - 1] = isr;
}

void interrupt_enable(index_t index) {
	uint8_t ri;

	if (!IS_INTERRUPT(index)) {
		ERROR();
		return;
	}

	ri = ifipin_to_pin[index - 1];

	pin_set_io(index, false);
	
	// unmask the interrupt.
	EXTI->IMR |= (1 << ri);

	// mask the event reqest.
	EXTI->EMR &= ~(1 << ri);
	
	// enable rising trigger and falling trigger.
	EXTI->RTSR |= (1 << ri);
	EXTI->FTSR |= (1 << ri);
}

void interrupt_disable(index_t index) {
	if (!IS_INTERRUPT(index)) {
		ERROR();
		return;
	}

	EXTI->IMR &= ~(1 << ifipin_to_pin[index]);
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
