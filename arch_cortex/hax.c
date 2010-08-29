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
 * INTERNAL FUNCTIONS
 */
void hw_setup1(void)
{
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

	spi_packet_init_u2m(&u2m);
	spi_packet_init_m2u(&m2u);

	printf("[ INIT DONE ]\n");
}

void hw_setup2(void)
{
	while(!is_master_ready());
}

void hw_spin(void)
{
}

void hw_loop1(void)
{
	vex_spi_xfer(&m2u, &u2m);
	spi_transfer_flag = false;
}

void hw_loop2(void)
{
}

bool hw_ready(void)
{
	return spi_transfer_flag;
}

mode_t mode_get(void)
{
	if (m2u.m2u.sys_flags.b.autonomus) {
		return MODE_AUTON;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return MODE_DISABLE;
	} else {
		return MODE_TELOP;
	}
}

/*
 * DIGITAL IO
 */
void digital_init(index_t pin, bool output)
{
}

bool digital_get(index_t index)
{
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (index) {
        /* VEXNet Joystick */
	case PIN_OI(5): /* Left trigger */
		return (joystick->g5_u << 0)
		     | (joystick->g5_d << 1);

	case PIN_OI(6): /* Right trigger */
		return (joystick->g6_u << 0)
		     | (joystick->g6_d << 1);

	case PIN_OI(7): /* Left Buttons */
		return (joystick->g7_u << 0)
		     | (joystick->g7_d << 1)
		     | (joystick->g7_l << 2)
		     | (joystick->g7_r << 3);

	case PIN_OI(8): /* Right Buttons */
		return (joystick->g8_u << 0)
		     | (joystick->g8_d << 1)
		     | (joystick->g8_l << 2)
		     | (joystick->g8_r << 3);

        /* Exposed Pins */
	case PIN_DIGITAL(1):
        case PIN_DIGITAL(2):
        case PIN_DIGITAL(3):
        case PIN_DIGITAL(4):
        case PIN_DIGITAL(5):
        case PIN_DIGITAL(6):
        case PIN_DIGITAL(7):
        case PIN_DIGITAL(8):
        case PIN_DIGITAL(9):
        case PIN_DIGITAL(10):
        case PIN_DIGITAL(11):
        case PIN_DIGITAL(12):
                return ifipin_to_port[index - 1];

	default:
		ERROR();
		return 0;
	}
}

/*
 * ANALOG IO
 */
void analog_set(index_t index, int8_t value)
{
        uint8_t value2;

        /* Convert the motor speed to an unsigned value. */
        value  = (value < 0 && value != -128) ? value - 1 : value;
        value2 = value + 128;

        switch (index) {
        /* Three-wire Motor/Servo*/
        case PIN_MOTOR(2):
        case PIN_MOTOR(3):
        case PIN_MOTOR(4):
        case PIN_MOTOR(5):
        case PIN_MOTOR(6):
        case PIN_MOTOR(7):
        case PIN_MOTOR(8):
        case PIN_MOTOR(9):
            u2m.u2m.motors[index] = val;
            break;

        /* Two-wire Motor */
        case PIN_MOTOR(1):
        case PIN_MOTOR(10):
            ERROR();
            break;

        default:
            ERROR();
        }
}


int16_t analog_get(index_t id)
{
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (id) {
        /* VEXNet Joystick */
        case PIN_OI(1): /* Right Stick, X */
		return joystick->axis_2;

	case PIN_OI(2): /* Right Stick, Y */
		return joystick->axis_1;

	case PIN_OI(4): /* Left Stick, X */
		return joystick->axis_4;

	case PIN_OI(3): /* Left Stick, Y */
		return joystick->axis_3;

        /* ADCs */
	case PIN_DIGITAL(1):
        case PIN_DIGITAL(2):
        case PIN_DIGITAL(3):
        case PIN_DIGITAL(4):
        case PIN_DIGITAL(5):
        case PIN_DIGITAL(6):
        case PIN_DIGITAL(7):
        case PIN_DIGITAL(8):
		return adc_buffer[index - 1];

    	default:
		ERROR();
		return 0;
	}
}

/*
 * INTERRUPTS
 */
void interrupt_init(index_t pin, isr_t isr)
{
}

void interrupt_set(index_t pin, bool enable)
{
}
