#include <hax.h>
#include <stdio.h>


#include "encoder.h"
#include "ports.h"
#include "util.h"

#include "robot.h"

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

void drive_raw(AnalogOut forward, AnalogOut strafe, AnalogOut turn) {
	int16_t f = (int16_t)-strafe  - turn;
	int16_t r = (int16_t)-forward - turn;
	int16_t b = (int16_t) strafe  - turn;
	int16_t l = (int16_t) forward - turn;
	int16_t max = max4(ABS(l), ABS(r), ABS(b), ABS(f));

	/* Scale the values to not exceed kMotorMax. */
	if (max > kMotorMax) {
		l = l * kMotorMax / max;
		r = r * kMotorMax / max;
		b = b * kMotorMax / max;
		f = f * kMotorMax / max;
	}

	motor_set(MTR_DRIVE_L, -l);
	motor_set(MTR_DRIVE_R, -r);
	motor_set(MTR_DRIVE_B, b);
	motor_set(MTR_DRIVE_F, f);
}

bool arm_raw(AnalogOut vel) {
	int16_t pos  = analog_adc_get(ANA_POT_ARM);
	bool    up   = vel > 0 && pos > ANA_POT_ARM_HIGH;
	bool    down = vel < 0 && pos < ANA_POT_ARM_LOW;
	bool    move = up || down;

	motor_set(MTR_ARM_L, move * -vel);
	motor_set(MTR_ARM_R, move * +vel);
	return !move;
}

bool ramp_raw(AnalogOut vel) {
	int16_t left  = analog_adc_get(SEN_POT_SCISSOR_L);
	int16_t right = analog_adc_get(SEN_POT_SCISSOR_R);

	bool mv_left  = (left  < SEN_POT_SCISSOR_L_HIGH && vel > 0)
	             || (left  > SEN_POT_SCISSOR_L_LOW  && vel < 0);
	bool mv_right = (right < SEN_POT_SCISSOR_R_HIGH && vel > 0)
	             || (right > SEN_POT_SCISSOR_R_LOW  && vel < 0);

	motor_set(MTR_SCISSOR_L, +vel * mv_left);
	motor_set(MTR_SCISSOR_R, -vel * mv_right);

	return !(mv_left || mv_right);
}

int32_t drive_straight(AnalogOut forward) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t diff  = ABS(right - left);
	int32_t error = PROP(ABS(forward), DRIVE_STRAIGHT_ERRMAX, diff);

	drive_raw(forward, 0, SIGN(forward) * SIGN(right - left) * error);

	return (left + right) / (2 * ENC_PER_IN);
}

bool arm_set(uint16_t tar) {
	uint16_t cur  = analog_adc_get(ANA_POT_ARM);
	uint16_t diff = ABS((int16_t)tar - (int16_t)cur);
	int8_t   out  = PROP(kMotorMax, ARM_SET_ERRMAX, diff);

	arm_raw(SIGN(tar - cur) * out);

	return diff < ARM_SET_ERRMAX;
}
