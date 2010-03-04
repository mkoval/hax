#include <stdio.h>
#include "stdbool.h"
#include "auton.h"
#include "hax.h"
#include "user.h"
#include "util.h"
#include "ports.h"

uint8_t kNumAnalogInputs = 6;

void isr_test(int8_t level) {
	_putc('!');
	_puts("INTERRUPTS\n");
}

volatile static long count[3];

#define ENCODER(_flip_,_other_,_num_)     \
	do {                                  \
		bool other = digital_get(_other_);\
		if (_flip_) {                     \
			if (other)                    \
				count[_num_]--;           \
			else                          \
				count[_num_]++;           \
		} else {                          \
			if (other)                    \
				count[_num_]++;           \
			else                          \
				count[_num_]--;           \
		}                                 \
	} while (0)

void encoder_0a(int8_t l) {
	ENCODER( l,17,0); // 2
}

void encoder_0b(int8_t l) {
	ENCODER(!l,16,0); // 3
}

void encoder_1a(int8_t l) {
	ENCODER( l,19,1); // 4
}

void encoder_1b(int8_t l) {
	ENCODER(!l,18,1); // 5
}

void encoder_2a(int8_t l) {
	ENCODER( l,21,2); // 6
}

void encoder_2b(int8_t l) {
	ENCODER(!l,20,2); // 7
}

void init(void) {
	_puts("Initialization\n");

	/* Test interrupts. */
	interrupt_reg_isr(0, encoder_0a);
	interrupt_reg_isr(1, encoder_0b);
	interrupt_reg_isr(2, encoder_1a);
	interrupt_reg_isr(3, encoder_1b);
	interrupt_reg_isr(4, encoder_2a);
	interrupt_reg_isr(5, encoder_2b);
	interrupt_enable(0);
	interrupt_enable(1);
	interrupt_enable(2);
	interrupt_enable(3);
	interrupt_enable(4);
	interrupt_enable(5);

	/* Use a jumper to test autonomous mode. */
	if (!digital_get(SEN_AUTON)) {
		printf((char *)"[AUTON forced]\n");
		mode_set(kAuton);
	}
}

void auton_loop(void) {
	uint8_t  i = 0;
	uint16_t t = 0;
	printf((char *)"[MODE auton]\n");

	++t;
	if (t >= 2200) {
		t = 0;
		mode_set(kTelop);
	}

	/* Reset all motor values. */
	for (i = 0; i < 8; ++i) {
		motor_set(i, 0);
	}

	auton_do();
}
	
void auton_spin(void) {
}

int8_t button(int8_t v) {
	if ( v > 50 )
		return 1;
	else if ( v < -50 )
		return -1;
	else
		return 0;
}

static int16_t max4(int16_t a, int16_t b, int16_t c, int16_t d) {
	if (a >= b) {
		if (a >= c) {
			if (a >= d) {
				return a;
			} else {
				return d;
			}
		} else {
			if (c >= d) {
				return c;
			} else {
				return d;
			}
		}
	} else {
		if (b >= c) {
			if (b >= d) {
				return b;
			} else {
				return d;
			}
		} else {
			if (c >= d) {
				return c;
			} else {
				return d;
			}
		}
	}
}

void drive_omni(int8_t x, int8_t y, int8_t omega) {
	int16_t f = (int16_t)-x - omega;
	int16_t r = (int16_t)-y - omega;
	int16_t b = (int16_t) x - omega;
	int16_t l = (int16_t) y - omega;
	int16_t max = max4(ABS(l), ABS(r), ABS(b), ABS(f));

	/* Scale the values to not exceed kMotorMax. */
	if (max > kMotorMax) {
		l = l * kMotorMax / max;
		r = r * kMotorMax / max;
		b = b * kMotorMax / max;
		f = f * kMotorMax / max;
	}

	motor_set(MTR_DRIVE_L, l);
	motor_set(MTR_DRIVE_R, r);
	motor_set(MTR_DRIVE_B, b);
	motor_set(MTR_DRIVE_F, f);
}

bool lift_arm(int8_t pwr) {
	int16_t pos = analog_adc_get(SEN_POT_ARM);

	bool move = (pos > SEN_POT_ARM_HIGH && pwr > 0)
	         || (pos < SEN_POT_ARM_LOW  && pwr < 0);
	
	motor_set(MTR_ARM_L, -pwr * move);
	motor_set(MTR_ARM_R, +pwr * move);

	return move;
}

bool lift_basket(int8_t pwr) {
	int16_t left  = analog_adc_get(SEN_POT_SCISSOR_L);
	int16_t right = analog_adc_get(SEN_POT_SCISSOR_R);

	bool mv_left  = (left  < SEN_POT_SCISSOR_L_HIGH && pwr > 0)
	             || (left  > SEN_POT_SCISSOR_L_LOW  && pwr < 0);
	bool mv_right = (right < SEN_POT_SCISSOR_R_HIGH && pwr > 0)
	             || (right > SEN_POT_SCISSOR_R_LOW  && pwr < 0);

	motor_set(MTR_SCISSOR_L, +pwr * mv_left);
	motor_set(MTR_SCISSOR_R, -pwr * mv_right);

	return mv_left || mv_right;
}


void telop_loop(void) {
	int8_t fwrd = analog_oi_get(OI_L_Y);
	int8_t side = analog_oi_get(OI_L_X); 
	int8_t spin = analog_oi_get(OI_R_X);
	int8_t lift = button(analog_oi_get(OI_R_B)) * kMotorMax;
	int8_t arm  = button(analog_oi_get(OI_L_B)) * kMotorMax;
	uint8_t i;

	/* IR Sensor calibration. */
#if 0
	printf((char *)"%u,%u,%u,%u,%u,%u\n",
		analog_adc_get(0),
		analog_adc_get(1),
		analog_adc_get(2),
		analog_adc_get(3),
		analog_adc_get(4),
		analog_adc_get(5)
	);
#endif

	/* Reset all motor values. */
	for (i = 0; i < 8; ++i) {
		motor_set(i, 0);
	}

	//printf((char *)"Encoders: %li %li %li\n",count[0],count[1],count[2]);

	/* Give the user direct control over the robot for now. */
	drive_omni(side, fwrd, spin);
	lift_arm(arm);
	lift_basket(lift);

	cruise();
}

void telop_spin(void) {
}

void disable_spin(void) {
}

void disable_loop(void) {
}
