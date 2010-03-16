#include "../hax.h"

#include "encoder.h"
#include "ports.h"
#include "util.h"

#include "robot.h"

void drive(AnalogOut forward, AnalogOut strafe, AnalogOut turn) {
	int16_t left  = (int16_t) forward - turn;
	int16_t right = (int16_t)-forward - turn;
	int16_t max   = MAX(left, right);

	if (max > kMotorMax) {
		left  = left  * kMotorMax / max;
		right = right * kMotorMax / max;
	}

	motor_set(MTR_DRIVE_FL, left);
	motor_set(MTR_DRIVE_BL, left);
	motor_set(MTR_DRIVE_FR, right);
	motor_set(MTR_DRIVE_BR, right);
	motor_set(MTR_STRAFE_F, strafe);
}

bool arm(AnalogOut vel) {
	int16_t pos  = analog_adc_get(ANA_POT_ARM);
	bool    up   = vel > 0 && pos < ANA_POT_ARM_HIGH;
	bool    down = vel < 0 && pos > ANA_POT_ARM_LOW;
	bool    move = up || down;

	motor_set(MTR_ARM_L, move *  vel);
	motor_set(MTR_ARM_R, move * -vel);
	return !move;
}

bool ramp(AnalogOut vel) {
	bool up   = vel > 0 && digital_get(DIG_LIM_LIFT_H);
	bool down = vel < 0 && digital_get(DIG_LIM_LIFT_L);
	bool move = up || down;

	motor_set(MTR_RAMP, move * vel);
	return !move;
}

int32_t drive_straight(AnalogOut forward) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t diff  = ABS(left - right);
	int32_t error = PROP(forward, DRIVE_STRAIGHT_ERRMAX, diff);

	/* Veering to the right; turn counter-clockwise (left) to compensate. */
	if (left > right) {
		drive(kMotorMax, 0, error);
	}
	/* Veering to the left; turn clockwise (right) to compensate. */
	else {
		drive(kMotorMax, 0, -error);
	}
	return MIN(left, -right);
}

void arm_set(uint16_t tar) {
	uint16_t cur  = analog_adc_get(ANA_POT_ARM);
	uint16_t diff = ABS((int16_t)tar - (int16_t)cur);
	int8_t   out  = PROP(kMotorMax, ARM_SET_ERRMAX, diff);

	if (cur < tar) {
		arm(out);
	} else {
		arm(-out);
	}
}
