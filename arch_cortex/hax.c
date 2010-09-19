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
	nvic_init();
	usart_init();
	spi_init();
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

ctrl_mode_t ctrl_mode_get(void) {
	if (m2u.m2u.sys_flags.b.autonomus) {
		return MODE_AUTON;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return MODE_DISABLE;
	} else {
		return MODE_TELOP;
	}
}

/*
 * OI access
 */

int8_t oi_group_get(index_t ix)
{
	uint8_t gr = IX_OI_GROUP_INV(ix) + 1;
	uint8_t oi_i = IX_OI_OI_INV(ix);
	if (oi_i != 0 && oi_i != 1) {
		WARN("ix: %d; gr: %d; oi: %d", ix, gr, oi_i);
		return 0;
	}
	struct oi_data *oi = &m2u.m2u.joysticks[oi_i].b;
	uint8_t data;
	switch(gr) {
	case 1:
		data = oi->axis_1;
		break;
	case 2:
		data = oi->axis_2;
		break;
	case 3:
		data = oi->axis_3;
		break;
	case 4:
		data = oi->axis_4;
		break;
	/* Triggers (groups of 2) */
	case 5:
		return 127*(oi->g5_u) - 127*(oi->g5_d);
	case 6:
		return 127*(oi->g6_u) - 127*(oi->g6_d);
	/* Buttons (groups of 4) */
	case 7:
	case 8:
		goto error;

	/* accelerometer data */
	case 9:
		data = oi->accel_x;
		break;
	case 10:
		data = oi->accel_y;
		break;
	case 11:
		data = oi->accel_z;
		break;
	default:
	error:
		WARN("ix: %d; gr: %d; oi: %d", ix, gr, oi_i);
		return 0;
	}

	return (data == -128)?(-127):(data);
}

#define ROCKER_CASE(x, y)       \
	case IX_OI_GROUP(x, y): \
		return oi[x-1]->g##y##_u - oi[x-1]->g##y##_d

int8_t oi_rocker_get(index_t ix)
{
	struct oi_data *oi [] = { &m2u.m2u.joysticks[0].b, &m2u.m2u.joysticks[1].b };
	switch(ix) {
	ROCKER_CASE(1,5);
	ROCKER_CASE(1,6);
	ROCKER_CASE(2,5);
	ROCKER_CASE(2,6);

	/* These ones are questionable */
	ROCKER_CASE(1,7);
	ROCKER_CASE(1,8);
	ROCKER_CASE(2,7);
	ROCKER_CASE(2,8);
	default:
		WARN_IX(ix);
		return 0;
	}
}

bool oi_button_get(index_t ix)
{
	uint8_t oi_i = IX_OI_BUTTON_OI_INV(ix);
	if (oi_i != 0 && oi_i != 1) {
		WARN("ix: %d; oi: %d", ix, oi_i);
		return false;
	}
	struct oi_data *oi = &m2u.m2u.joysticks[oi_i].b;
	index_t i = IX_OI_BUTTONx_INV(ix, oi_i);

	switch(i) {
	case IX_OI_BUTTON(1, 5, OI_B_UP):
		return oi->g5_u;
	case IX_OI_BUTTON(1, 5, OI_B_DN):
		return oi->g5_d;
	case IX_OI_BUTTON(1, 6, OI_B_UP):
		return oi->g6_u;
	case IX_OI_BUTTON(1, 6, OI_B_DN):
		return oi->g6_d;

	case IX_OI_BUTTON(1, 7, OI_B_UP):
		return oi->g7_u;
	case IX_OI_BUTTON(1, 7, OI_B_LT):
		return oi->g7_l;
	case IX_OI_BUTTON(1, 7, OI_B_DN):
		return oi->g7_d;
	case IX_OI_BUTTON(1, 7, OI_B_RT):
		return oi->g7_r;

	case IX_OI_BUTTON(1, 8, OI_B_UP):
		return oi->g8_u;
	case IX_OI_BUTTON(1, 8, OI_B_LT):
		return oi->g8_l;
	case IX_OI_BUTTON(1, 8, OI_B_DN):
		return oi->g8_d;
	case IX_OI_BUTTON(1, 8, OI_B_RT):
		return oi->g8_r;

	default:
		WARN_IX(ix);
		return false;
	}
}
