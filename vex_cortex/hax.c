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
	
	spi_packet_vex m2u, u2m;

	memset(&u2m,0,sizeof(u2m));
	memset(&m2u,0,sizeof(m2u));

	spi_packet_init_u2m(&u2m);
	spi_packet_init_m2u(&m2u);
	
	printf("[ INIT DONE ]\n");
	for(;;) {
		if (spi_transfer_flag) {
			vex_spi_xfer(&m2u,&u2m);
			spi_transfer_flag = false;
		}
	}
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
	/* PE9, PE11,  PC6,  PC7, PE13, PE14,  PE8, PE10, PE12,  PE7,  PD0,  PD1*/
static const GPIO_TypeDef *gpio_ports[12] = 
	{GPIOE,GPIOE,GPIOC,GPIOC,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOD,GPIOD};
static const int8_t   gpio_index[12] =
	{    9,   11,    6,    7,   13,   14,    8,   10,   12,    7,    0,    1};

void pin_set_io(PinIx pin_index, PinMode pin_mode) {	
	GPIO_InitTypeDef GPIO_param;

	if (pin_index >= 12) {
		return;
	}
	
	GPIO_param.GPIO_Pin =
		(uint16_t)(1 << gpio_index[pin_index]);
	
	if (pin_mode == kInput) {
		GPIO_param.GPIO_Mode = GPIO_Mode_IPU;
	} else if (pin_mode == kOutput) {
		GPIO_param.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_param.GPIO_Mode = GPIO_Mode_Out_PP;
	}
	
	GPIO_Init((GPIO_TypeDef *)gpio_ports[pin_index],
		&GPIO_param);	
}

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

bool digital_get(PinIx index) {
	uint8_t ret = GPIO_ReadInputDataBit(
		(GPIO_TypeDef*)gpio_ports[index],
		1<<gpio_index[index]);
		
	return ret != 0;
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
void interrupt_reg_isr(InterruptIx index, InterruptServiceRoutine isr) {
	/* TODO */
}

bool interrupt_get(InterruptIx index) {
	return false;
}

void interrupt_enable(InterruptIx index) {
	/* TODO */
}

void interrupt_disable(InterruptIx index) {
	/* TODO */
}

/*
 * SERIAL IO
 */
void _putc(char c) {
	printf("%c", c);
}
