#include <hax.h>
#include <stdio.h>

#include "encoder.h"
#include "ports.h"
#include "util.h"

#include "robot.h"

void drive_raw(AnalogOut forward, AnalogOut turn) {
	int16_t left  = forward - turn;
	int16_t right = forward + turn;
	int16_t max   = MAX(left, right);

	/* Scale the values to not exceed kMotorMax. */
	if (max > kMotorMax) {
		left  = left  * kMotorMax / max;
		right = right * kMotorMax / max;
	}

	motor_set(MTR_DRIVE_L, left);
	motor_set(MTR_DRIVE_R, right);
}

bool arm_raw(AnalogOut vel) {
	int16_t pos  = analog_adc_get(POT_ARM);
	bool    up   = vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = vel < 0 && ARM_GT(pos, POT_ARM_LOW);
	bool    move = up || down;

	motor_set(MTR_ARM_A, move * -vel);
	motor_set(MTR_ARM_B, move * +vel);
	return !move;
}

bool ramp_raw(AnalogOut vel) {
#if 0
	int16_t left     = analog_adc_get(POT_LIFT_L);
	int16_t right    = analog_adc_get(POT_LIFT_R);

	bool mv_left  = (LIFT_L_LT(left, POT_LIFT_L_HIGH) && vel > 0)
	             || (LIFT_L_GT(left, POT_LIFT_L_LOW)  && vel < 0);
	
	bool mv_right = (LIFT_R_LT(right, POT_LIFT_R_HIGH) && vel > 0)
	             || (LIFT_R_GT(right, POT_LIFT_R_LOW)  && vel < 0);
#endif

	/* TODO Figure out how we're detecting the ramp's position. */
	bool    mv_left  = false;
	bool    mv_right = false;
	motor_set(MTR_LIFT_L, +vel * mv_left);
	motor_set(MTR_LIFT_R, -vel * mv_right);

	return !(mv_left || mv_right);
}

int32_t drive_straight(AnalogOut forward) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t diff  = ABS(right - left);
	int32_t error = PROP(ABS(forward), DRIVE_STRAIGHT_ERRMAX, diff);

	drive_raw(forward, SIGN(forward) * SIGN(right - left) * error);

	return (left + right) / 2;
}

int32_t drive_turn(AnalogOut turn) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t avg   = ABS(left - right) / 2;

	drive_raw(0, turn);

	return SIGN(turn) * avg;
}
