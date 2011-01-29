#ifndef CORTEX_H_
#define CORTEX_H_

#include <stm32f10x.h>
#include <stdint.h>
/** RCC */

/* CIR */
#define RCC_CIR_CSSF_B 7
#define RCC_CIR_CSSC_B 23

/* CR */
#define RCC_CR_CSSON_B 19
#define RCC_CR_HSEON_B 16


//#define PERIPH_BB(reg, pin) (*(PERIPH_BB_BASE + (&(reg) - PERIPH_BASE) * 32 + (pin * 4)))

#define PERIPH_BIT(system, regn, bitn) (*((uint32_t *)(PERIPH_BB_BASE + \
			(((uintptr_t)&((system)->regn) - PERIPH_BASE) * 32) \
			        + (system##_##regn##_##bitn##_B))))

#endif
