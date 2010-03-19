#include <stdio.h>
#include <stdbool.h>
#include "auton.h"
#include "hax.h"
#include "user.h"
#include "util.h"
#include "ports.h"
#include "mikish/encoder.h"

uint8_t kNumAnalogInputs = 6;
bool auton = false;

void init(void) {
	_puts("Initialization\n");

	/* Enable left and right wheel interrupts. */
	interrupt_reg_isr(2 * INT_ENC_L + 0, encoder_0a);
	interrupt_reg_isr(2 * INT_ENC_L + 1, encoder_0b);
	interrupt_reg_isr(2 * INT_ENC_R + 0, encoder_1a);
	interrupt_reg_isr(2 * INT_ENC_R + 1, encoder_1b);

	interrupt_enable(2 * INT_ENC_L + 0);
	interrupt_enable(2 * INT_ENC_L + 1);
	interrupt_enable(2 * INT_ENC_R + 0);
	interrupt_enable(2 * INT_ENC_R + 1);
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

void drive_omni(int8_t x, int8_t y, int8_t z) {
	int16_t a = (int16_t) y - z;
	int16_t b = (int16_t) x;
	int16_t c = (int16_t)-y - z;

	int16_t max = max4(ABS(a), ABS(b), ABS(c), 0);

	/* Scale the values to not exceed kMotorMax. */
	if (max > kMotorMax) {
		a = a * kMotorMax / max;
		b = b * kMotorMax / max;
		c = c * kMotorMax / max;
	}

	motor_set(3, a);
	motor_set(2, a);
	motor_set(4, b);
	motor_set(0, c);
	motor_set(1, c);
}

#if 0
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
#endif

void auton_loop(void) {
	int32_t left  = encoder_get(INT_ENC_L);
	int32_t right = encoder_get(INT_ENC_R);
	uint8_t i     = 0;
	int32_t omega;

	puts((char *)"[MODE auton]");

	/* Reset all motor values. */
	for (i = 0; i < 8; ++i) {
		motor_set(i, 0);
	}
	
#if 0
	omega = SIGN(left + right) * PROP_SCALE(kMotorMax, 250, ABS(left + right));
	printf((char *)"L %10ld R %10ld W %10ld\n", left, right, omega);
	drive_omni(0, kMotorMin, omega);
#endif
}
	
void auton_spin(void) {
}

void telop_loop(void) {
	int8_t fwrd = analog_oi_get(OI_L_Y);
	int8_t side = analog_oi_get(OI_L_X); 
	int8_t spin = analog_oi_get(OI_R_X);
	int8_t lift = button(analog_oi_get(OI_R_B)) * kMotorMax;
	int8_t arm  = button(analog_oi_get(OI_L_B)) * kMotorMax;
	uint8_t i;

	puts((char *)"[MODE telop]");

	/* Reset all motor values. */
	for (i = 0; i < 8; ++i) {
		motor_set(i, 0);
	}

	/* Give the user direct control over the robot for now. */
	drive_omni(side, fwrd, spin);
	//lift_arm(arm);
	//lift_basket(lift);
}

void telop_spin(void) {
}

void disable_spin(void) {
}

void disable_loop(void) {
	int32_t left  = encoder_get(INT_ENC_L);
	int32_t right = encoder_get(INT_ENC_R);
	int32_t omega;

	omega = SIGN(left + right) * PROP_SCALE(kMotorMax, 250, ABS(left + right));
	printf((char *)"L %10ld R %10ld W %10ld\n", left, right, omega);
}
