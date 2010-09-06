#include <hax.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f10x.h"
#include "vex_hw.h"

#if defined(USE_STDPERIPH_DRIVER)
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_misc.h"
#include "core_cm3.h"
#endif

#include "compilers.h"
#include "init.h"
#include "rcc.h"
#include "usart.h"
#include "spi.h"
#include "exti.h"

spi_packet_t m2u;
spi_packet_t u2m;

/*
 * INTERNAL FUNCTIONS
 */
void arch_init_1(void) {
	rcc_init();
	gpio_init();
	usart_init();
	spi_init();
	nvic_init();
	tim1_init();
	adc_init();
	exti_init();

	memset(&u2m, 0, sizeof u2m);
	memset(&m2u, 0, sizeof m2u);

	spi_packet_init_u2m(&u2m.u2m);
	spi_packet_init_m2u(&m2u.m2u);

	printf("[ INIT DONE ]\n");
}

void arch_init_2(void) {
	while(!is_master_ready());
}

void arch_spin(void) {
}

void arch_loop_1(void) {
	vex_spi_xfer(&m2u.m2u, &u2m.u2m);
	spi_transfer_flag = false;
}

void arch_loop_2(void) {
}

bool do_slow_loop(void) {
	return spi_transfer_flag;
}

ctrl_mode_t mode_get(void) {
	if (m2u.m2u.sys_flags.b.autonomus) {
		return MODE_AUTON;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return MODE_DISABLE;
	} else {
		return MODE_TELOP;
	}
}
