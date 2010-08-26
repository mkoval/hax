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

/* Derived from TIM1 */
uint16_t const kSlowSpeed = 18200;

spi_packet_vex m2u;
spi_packet_vex u2m;

/*
 * INITIALIZATION AND MISC
 */
void setup_1(void) {
	rcc_init();
	gpio_init();
	usart_init();
	spi_init();
	nvic_init();
	tim1_init();
	adc_init();
	exti_init();
	
	memset(&u2m,0,sizeof(u2m));
	memset(&m2u,0,sizeof(m2u));

	spi_packet_init_u2m(&u2m);
	spi_packet_init_m2u(&m2u);
	
	printf("[ INIT DONE ]\n");
}

void setup_2(void) {	
	while(!is_master_ready());
}

void spin(void) {
}

void loop_1(void) {
	vex_spi_xfer(&m2u, &u2m);
	spi_transfer_flag = false;
}

void loop_2(void) {
}

bool new_data_received(void) {
	return spi_transfer_flag;
}

state_t mode_get(void) {
	if (m2u.m2u.sys_flags.b.autonomus) {
		return MODE_AUTON;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return MODE_DISABLE;
	} else {
		return MODE_TELOP;
	}
}

/*
 * ANALOG AND DIGITAL INPUTS
 */
int8_t analog_oi_get(index_t button) {
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;
	uint16_t sp = 0;

	switch (button) {
	case 1: /* Left Stick, X */
		sp = joystick->axis_4;
		break;
	case 2: /* Left Stick, Y */
		sp = joystick->axis_3;
		break;
	case 3: /* Right Stick, X */
		sp = joystick->axis_2;
		break;
	case 4: /* Right Stick, Y */
		sp = joystick->axis_1;
		break;
	default:
		ERROR();
		return 0;
	}

	sp = (sp < 0 && sp != -128) ? sp - 1 : sp;
	return sp + 128;
}

uint16_t analog_adc_get(index_t index) {
	/* Pretend the Cortex has the same precision as the PIC. */
	return adc_buffer[index - 1] >> 2;
}

bool digital_oi_get(index_t index) {
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (index) {
	case 5: /* Left Buttons, Up */
		return joystick->g8_u;
	case 6: /* Left Buttons, Down */
		return joystick->g8_d;
	case 7: /* Left Buttons, Left */
		return joystick->g8_l;
	case 8: /* Left Buttons, Right */
		return joystick->g8_r;
	case 9: /* Right Buttons, Up */
		return joystick->g7_u;
	case 10: /* Right Buttons, Down */
		return joystick->g7_d;
	case 11: /* Right Buttons, Left */
		return joystick->g7_l;
	case 12: /* Right Buttons, Right */
		return joystick->g7_r;
	case 13: /* Left Trigger, Up */
		return joystick->g5_u;
	case 14: /* Left Trigger, Down */
		return joystick->g5_d;
	case 15: /* Right Trigger, Up */
		return joystick->g6_u;
	case 16: /* Right Trigger, Down */
		return joystick->g6_d;
	default:
		ERROR();
		return false;
	}
}

/*
 * MOTOR AND SERVO OUTPUTS
 */
void analog_set(index_t index, int8_t sp) {
	/* Two-wire motors. */
	if (index == 1 || index == 10) {
		/* TODO Two wire motor support. */
	}
	/* Three-wire servo or servomotor */
	else if (2 <= index && index <= 9) {
		uint8_t val;
		sp = (sp < 0 && sp != -128) ? sp - 1 : sp;
		val = sp + 128;

		u2m.u2m.motors[index] = val;
	} else {
		ERROR();
	}
}

/*
 * SERIAL IO
 */
void _putc(char c) {
	putchar(c);
}
