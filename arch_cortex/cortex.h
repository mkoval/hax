#ifndef CORTEX_H_
#define CORTEX_H_

#include <stm32f10x.h>
#include <stdint.h>
/** RCC **/

/* CIR */
#define RCC_CIR_CSSF_B 7
#define RCC_CIR_CSSC_B 23

/* CR */
#define RCC_CR_CSSON_B 19
#define RCC_CR_HSEON_B 16

/* APB1ENR */
#define RCC_APB1ENR_TIM4EN_B 2


/** AFIO **/
/* MAPR */
#define AFIO_MAPR_TIM4_REMAP_B 12


/** GPIO **/
#define GPIO_CR_CNF_MASK
#define GPIO_CR_MODE_MASK


//#define PERIPH_BB(reg, pin) (*(PERIPH_BB_BASE + (&(reg) - PERIPH_BASE) * 32 + (pin * 4)))


/**
 * PERIPH_BIT_SET - sets a peripheral bit using the bit banding region
 * @system - subsystem name
 * @regn   - register name
 * @bitn   - bit name
 * @value  - value to set to the specified bit
 */
#define PERIPH_BIT_SET(system, regn, bitn, value) \
	((*((uint32_t *)(PERIPH_BB_BASE + (((uintptr_t)&((system)->regn) - PERIPH_BASE) * 32) \
			        + (system##_##regn##_##bitn##_B)))) = (value)?1:0)

#endif
