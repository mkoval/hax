#include <hax.h>
#include <stdio.h>

#include "encoder.h"
#include "ports.h"
#include "util.h"

#include "robot.h"

void drive_raw(AnalogOut left, AnalogOut right) {
	motor_set(MTR_DRIVE_L, +left);
	motor_set(MTR_DRIVE_R, -right);
}

void drive_smart(AnalogOut forward, AnalogOut turn) {
	int16_t left  = (int16_t) forward - turn;
	int16_t right = (int16_t) forward + turn;
	int16_t max   = MAX(ABS(left), ABS(right));

	/* Scale the values to not exceed kMotorMax. */
	if (max > kMotorMax) {
		left  = left  * kMotorMax / max;
		right = right * kMotorMax / max;
	}

	motor_set(MTR_DRIVE_L, -left);
	motor_set(MTR_DRIVE_R, +right);
}

void arm_raw(AnalogOut vel) {
	motor_set(MTR_ARM_A, +vel);
	motor_set(MTR_ARM_B, -vel);
}

bool arm_smart(AnalogOut vel) {
	int16_t pos  = analog_adc_get(POT_ARM);
	bool    up   = vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = vel < 0 && ARM_GT(pos, POT_ARM_LOW);
	bool    move = up || down;

	motor_set(MTR_ARM_A, move * +vel);
	motor_set(MTR_ARM_B, move * -vel);

	return !move;
}

void ramp_raw(AnalogOut vel) {
	motor_set(MTR_LIFT_L, -vel);
	motor_set(MTR_LIFT_R, +vel);
}

bool ramp_smart(AnalogOut vel) {
	int16_t pos  = analog_adc_get(POT_LIFT);
	bool    up   = vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	bool    move = up || down;

	motor_set(MTR_LIFT_L, -vel * move);
	motor_set(MTR_LIFT_R, +vel * move);

	return !move;
}

int32_t drive_straight(AnalogOut forward) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t diff  = ABS(right - left);
	int32_t error = PROP(ABS(forward), DRIVE_STRAIGHT_ERRMAX, diff);

	drive_smart(forward, SIGN(forward) * SIGN(right - left) * error);

	return (left + right) / 2;
}

int32_t drive_turn(AnalogOut turn) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t avg   = ABS(left - right) / 2;

	drive_smart(0, turn);

	return SIGN(turn) * avg;
}
