/* From system_stm32f10x.c,
 * STD PERIPH LIB 3.2.0
 */

#include <stm32f10x.h>

#include "cortex.h"
#include "clocks.h"

/* Tracks our clock source so we can guess at frequency
 *  HSE => 72 MHz
 *  HSI => 64 MHz
 */
#define CS_HSE 0
#define CS_HSI 1
static int rcc_src_hclk;

static volatile uint64_t jiffies;

#define jiffies_to_ms(jiffies) ((jiffies)/2)

/* acuracy is +- the size of a single clock tick. */
void udelay_500(void) {
	uint64_t target = jiffies + 1;
	while (jiffies <= target)
		;
}

uint64_t time_get_ms(void)
{
	return jiffies_to_ms(jiffies);
}

__attribute__((interrupt))
void SysTick_Handler(void)
{
	jiffies++;
}

/**
 * systick_setup - initialize the systick subsystem with a 500us period
 *	 72 * 1000 * 1000 / 8 / 4500 = 2000
 *	 64 * 1000 * 1000 / 8 / 4000 = 2000
 */
static void systick_setup(void)
{
	SysTick->CTRL = 0;

	if (rcc_src_hclk == CS_HSE) {
		SysTick->LOAD = 4500;
	} else if (rcc_src_hclk == CS_HSI) {
		SysTick->LOAD = 4000;
	}

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
		| SysTick_CTRL_TICKINT_Msk
		| SysTick_CTRL_ENABLE_Msk;
}

static void rcc_reset(void)
{
	/* Reset the RCC clock
	 * configuration to the
	 * default reset state
	 */
	/* Set HSION bit */
	RCC->CR |= RCC_CR_HSION;

	/* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#if !defined(STM32F10X_CL)
	RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
	RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif /* STM32F10X_CL */

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= ~(RCC_CR_HSEON
		| RCC_CR_CSSON
		| RCC_CR_PLLON);

	/* Reset HSEBYP bit */
	RCC->CR &= ~(RCC_CR_HSEBYP);

	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
	RCC->CFGR &= (uint32_t)0xFF80FFFF;
	/*
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC
		| RCC_CFGR_PLLXTPRE
		| RCC_CFGR_PLLMUL);
	*/

#if defined( STM32F10X_CL)
	/* Reset PLL2ON and PLL3ON bits */
	RCC->CR &= ~(RCC_CR_PLL2ON
		| RCC_CR_PLL3ON);
	/* Disable all interrupts and clear pending bits  */
	RCC->CIR = 0x00FF0000;

	/* Reset CFGR2 register */
	RCC->CFGR2 = 0x00000000;
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL)
	/* Disable all interrupts and clear pending bits  */
	RCC->CIR = 0x009F0000;

	/* Reset CFGR2 register */
	RCC->CFGR2 = 0x00000000;
#else
	/* Disable all interrupts and clear pending bits  */
	RCC->CIR = 0x009F0000;
#endif /* STM32F10X_CL */

}

static void pll_setup(void)
{
	/* PLL Config */
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC
		| RCC_CFGR_PLLXTPRE
		| RCC_CFGR_PLLMULL);

	if (rcc_src_hclk == CS_HSE) {
		/* PLLCLK = HSE * 9 = 72 MHz */
		RCC->CFGR |= RCC_CFGR_PLLSRC_HSE
			| RCC_CFGR_PLLMULL9;
	} else {
		/* PLLCLK = HSI / 2 * 16 = 64 MHz */
		RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_Div2
			| RCC_CFGR_PLLMULL16;
	}

	/* Enable PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till PLL is ready */
	while(!(RCC->CR & RCC_CR_PLLRDY));

	/* Select PLL as system clock source */
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/* Wait till PLL is used as system clock source */
	while ((RCC->CFGR & RCC_CFGR_SWS)
		!= RCC_CFGR_SWS_PLL);
}

__attribute__((interrupt))
void NMI_Handler(void)
{
	uint32_t cir = RCC->CIR;
	if (cir & RCC_CIR_CSSC) {
		/* HSE failed, switch to HSI */
		rcc_src_hclk = CS_HSI;

		pll_setup();
		systick_setup();

		/* clear the flag */
		PERIPH_BIT_SET(RCC, CIR, CSSC, 1);

	} else {
		/* Handle other NMI's */
	}
}

static void rcc_setup(void)
{
	// Assumptions on entry:
	//  running on HSI, PLL disabled.
	//  all peripheral clocks disabled.
	/** SYSCLK, HCLK, PCLK2 and PCLK1 configuration **/
	/* Enable HSE */
	PERIPH_BIT_SET(RCC, CR, HSEON, 1);

	/* Wait till HSE is ready and if Time out is reached exit */
	uint32_t i;
	for(i = 0;; i++) {
		if (i >= HSEStartUp_TimeOut) {
			rcc_src_hclk = CS_HSI;
			break;
		}
		if (RCC->CR & RCC_CR_HSERDY) {
			rcc_src_hclk = CS_HSE;
			/* enable clock security system */
			PERIPH_BIT_SET(RCC, CR, CSSON, 1);
			break;
		}
	}

	/* FLASH: Enable Prefetch Buffer
	 *  (note: should only be done when running off of 8Mhz HSI.) */
	FLASH->ACR |= FLASH_ACR_PRFTBE;

	/* FLASH: Set latency to 2 */
	FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_2;

	/* HCLK = SYSCLK */
	// HCLK: AHB (Max 72Mhz)
	//  = SYSCLK = 72Mhz
	// 0xxx: SYSCLK not divided
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV1;

	/* PCLK2 = HCLK */
	// PCLK2: APB2 (APB high speed, Max 72Mhz)
	//	= HCLK = 72MHz
	// 0xx: HCLK not divided
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | RCC_CFGR_PPRE2_DIV1;

	/* PCLK1 = HCLK/2 */
	// PCLK1: APB1 (APB low speed, Max 36Mhz)
	//	= HCLK/2 = 36MHz
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_CFGR_PPRE1_DIV2;

	pll_setup();

}

void clocks_init(void) {
	rcc_reset();
	rcc_setup();
	systick_setup();
}
