#include <hax.h>
#include <stdio.h>

#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "util.h"

#include "auton.h"

/* Drive straight using encoders for velocity control. */
void auto_straight_init(data_t *data) {
	data->move.enc_left  = encoder_get(ENC_L);
	data->move.enc_right = encoder_get(ENC_R);
}

void auto_straight_loop(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.enc_right;
	int32_t diff  = left - right;
	int8_t  turn  = PROP(ABS(data->move.vel), STRAIGHT_ERROR, ABS(diff));

	drive_smart(data->move.vel, SIGN(diff) * turn);
}

bool auto_straight_isdone(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.enc_right;

	return ABS(left + right) >= 2 * data->move.ticks;
}

/* Turn through a specified angle using encoders as velocity control. */
void auto_turn_init(data_t *data) {
	/* Store the initial encoder counts for both wheels. */
	data->move.enc_left  = encoder_get(ENC_L);
	data->move.enc_right = encoder_get(ENC_R);
}

void auto_turn_loop(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.enc_right;
	int32_t diff  = ABS(left - right) / 2 - data->move.ticks;
	int8_t  turn  = PROP(ABS(data->move.vel), TURN_ERROR, ABS(diff));

	drive_smart(0, SIGN(data->move.vel) * turn);
}

bool auto_turn_isdone(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.enc_right;
	int32_t diff  = ABS(left - right) / 2 - data->move.ticks;

	return ABS(diff) < TURN_THRESHOLD;
}

/* Rotate the arm to be in the specified position. */
void auto_arm_init(data_t *data) {
}

void auto_arm_loop(data_t *data) {
	int16_t pos  = analog_adc_get(POT_ARM);
	bool    up   = data->pose.vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = data->pose.vel < 0 && ARM_GT(pos, POT_ARM_LOW);
	bool    move = up || down;

	arm_raw(move * data->pose.vel);
}

bool auto_arm_isdone(data_t *data) {
	int16_t pos  = analog_adc_get(POT_ARM);
	bool    up   = data->pose.vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = data->pose.vel < 0 && ARM_GT(pos, POT_ARM_LOW);

	return !(up || down);
}

/* Move the ramp into the specific position.  */
void auto_ramp_init(data_t *data) {
}

void auto_ramp_loop(data_t *data) {
	int16_t pos  = analog_adc_get(POT_LIFT);
	bool    up   = data->pose.vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = data->pose.vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	bool    move = up || down;

	ramp_raw(move * data->pose.vel);
}

bool auto_ramp_isdone(data_t *data) {
	int16_t pos  = analog_adc_get(POT_LIFT);
	bool    up   = data->pose.vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = data->pose.vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	
	return !(up || down);
}

bool auto_ram_isdone(data_t *data) {
	return !digital_get(BUT_B);
}

/* Do nothing for a given timeout.  */
void auto_none_init(data_t *data) {
}

void auto_none_loop(data_t *data) {
}

bool auto_none_isdone(data_t *data) {
	return true;
}
