#include <stdio.h>
#include "hax.h"
#include "user.h"
#include "ports.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))

uint8_t kNumAnalogInputs = 5;

void init(void) {
	_puts("Initialization\n");
}

void auton_loop(void) {
	_puts("AUTO\n");
	/* there is no autonomous mode */
}
	
void auton_spin(void) {
	/* there is no autonomous mode */
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
	int16_t f = (int16_t) x + omega;
	int16_t r = (int16_t)-y + omega;
	int16_t b = (int16_t)-x + omega;
	int16_t l = (int16_t) y + omega;
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

void lift_arm(int16_t angle) {
	int16_t pos = analog_adc_get(SEN_POT_ARM);

	if (pos < angle) {
		motor_set(MTR_ARM_L, kMotorMax);
		motor_set(MTR_ARM_R, -kMotorMax);
	} else if (pos > angle) {
		motor_set(MTR_ARM_L, -kMotorMax);
		motor_set(MTR_ARM_R, kMotorMax);
	} else {
		motor_set(MTR_ARM_L, 0);
		motor_set(MTR_ARM_R, 0);
	}
}

void telop_loop(void) {
	int8_t fwrd = analog_oi_get(OI_L_Y);
	int8_t side = analog_oi_get(OI_L_X); 
	int8_t spin = analog_oi_get(OI_R_X);
	int8_t lift = button(analog_oi_get(OI_R_B)) * kMotorMax;
	int8_t arm  = button(analog_oi_get(OI_L_B)) * kMotorMax;

	printf((char *)"Arm: %d\n", analog_adc_get(SEN_POT_ARM));

	drive_omni(-side, fwrd, -spin);
	// lift_arm(arm);
}

void telop_spin(void) {
}

