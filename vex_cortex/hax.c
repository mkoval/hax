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

CtrlMode mode_get(void) {
	if (m2u.m2u.sys_flags.b.autonomus) {
		return kAuton;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return kDisable;
	} else {
		return kTelop;
	}
}

/*
 * ANALOG AND DIGITAL INPUTS
 */


enum {
	OI_STICK_L_X,
	OI_STICK_L_Y,
	OI_STICK_R_X,
	OI_STICK_R_Y,
	OI_BUT_L_U,
	OI_BUT_L_D,
	OI_BUT_L_L,
	OI_BUT_L_R,
	OI_BUT_R_U,
	OI_BUT_R_D,
	OI_BUT_R_L,
	OI_BUT_R_R,
	OI_TRIG_L_U,
	OI_TRIG_L_D,
	OI_TRIG_R_U,
	OI_TRIG_R_D
};

int8_t analog_oi_get(OIIx index) {
	struct oi_data *joystick = &m2u.m2u.joysticks[index / 20].b;
	int button = index % 20;

	switch (button) {
	/* Left Stick */
	case OI_STICK_L_X:
		return joystick->axis_4;
	case OI_STICK_L_Y:
		return joystick->axis_3;
	
	/* Right Stick */
	case OI_STICK_R_X:
		return joystick->axis_2;
	case OI_STICK_R_Y:
		return joystick->axis_1;
	
	/* Left Buttons */
	case OI_BUT_L_U:
		return joystick->g8_u;
	case OI_BUT_L_D:
		return joystick->g8_d;
	case OI_BUT_L_L:
		return joystick->g8_l;
	case OI_BUT_L_R:
		return joystick->g8_r;
	
	/* Right Buttons */
	case OI_BUT_R_U:
		return joystick->g7_u;
	case OI_BUT_R_D:
		return joystick->g7_d;
	case OI_BUT_R_L:
		return joystick->g7_l;
	case OI_BUT_R_R:
		return joystick->g7_r;
	
	/* Left Triggers */
	case OI_TRIG_L_U:
		return joystick->g5_u;
	case OI_TRIG_L_D:
		return joystick->g5_d;
	
	/* Right Triggers */
	case OI_TRIG_R_U:
		return joystick->g6_u;
	case OI_TRIG_R_D:
		return joystick->g6_d;
	
	default:
		return 0;
	}
}

uint16_t analog_adc_get(PinIx index) {
	return adc_buffer[index];
}

void digital_set(PinIx index, bool value) {
	/* TODO */
}



/*
 * MOTOR AND SERVO OUTPUTS
 */
void analog_set(AnalogOutIx index, AnalogOut value) {
	/* TODO */
}

void motor_set(AnalogOutIx index, MotorSpeed value) {
	analog_set(index, value);
}

void servo_set(AnalogOutIx index, ServoPosition value) {
	analog_set(index, value);
}

/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */

/*
 * SERIAL IO
 */
void _putc(char c) {
	putchar(c);
}
