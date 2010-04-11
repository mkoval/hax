#include <hax.h>
#include <stdio.h>

#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "util.h"

#include "auton.h"

data_t auto_straight_create(uint16_t timeout, uint16_t dist, int8_t speed) {
	data_t data;
	data.move.timeout = timeout;
	data.move.dist    = dist * ENC_PER_10IN;
	data.move.speed   = speed;
	return data;
}

data_t auto_ram_create(uint16_t timeout, int8_t speed) {
	data_t data;
	data.move.timeout = timeout;
	data.move.dist    = 0;
	data.move.vel     = speed;
	return data;
}

data_t auto_turn_create(uint16_t timeout, uint16_t angle, int8_t speed) {
	data_t data;
	data.move.timeout = timeout;
	data.move.dist    = angle * ENC_PER_DEG;
	data.move.vel     = speed;
	return data;
}

data_t auto_noop_create(uint16_t us) {
	data_t data;
	data.timeout = us / timeout;
	return data;
}

/* Drive straight using encoders for velocity control. */
void auto_straight_init(data_t *data) {
	/* Store the initial encoder counts for both wheels. */
	data->move.enc_left  = encoder_get(ENC_L);
	data->move.enc_right = encoder_get(ENC_R);
}

void auto_straight_loop(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.enc_right;
	int32_t diff  = left - right;
	int8_t  turn  = PROPTO(kMotorMax, STRAIGHT_ERROR, ABS(diff));

	drive_smart(data->move.vel, SIGN(diff) * turn);
	--data->timeout;
}

void auto_straight_isdone(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.end_right;

	return ABS(left + right) >= 2 * data->move.ticks;
}

/* Turn through a specified angle using encoders as velocity control. */
void auto_turn_init(data_t *data) {
	/* Store the initial encoder counts for both wheels. */
	data->move.enc_left  = encoder_get(ENC_L);
	data->move.enc_right = encoder_get(ENC_R);
}

void auto_straight_loop(data_t *data) {
	int32_t left  = encoder_get(ENC_L) - data->move.enc_left;
	int32_t right = encoder_get(ENC_R) - data->move.enc_right;
	int32_t diff  = ABS(left - right) / 2 - data->move.ticks;
	int8_t  turn  = PROPTO(ABS(data->move.vel), TURN_ERROR, ABS(diff));

	drive_smart(0, SIGN(data->move.vel) * turn);
	--data->timeout;
}

void auto_turn_isdone(data_t *data) {
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
	bool    up   = vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = vel < 0 && ARM_GT(pos, POT_ARM_LOW);
	bool    move = up || down;

	arm_raw(move * vel);
	--data->timeout;
}

bool auto_arm_isdone(data_t *data) {
	int16_t pos  = analog_adc_get(POT_ARM);
	bool    up   = vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = vel < 0 && ARM_GT(pos, POT_ARM_LOW);

	return !(up || down);
}

/* Move the ramp into the specific position.  */
void auto_ramp_init(data_t *data) {
}

void auto_ramp_loop(data_t *data) {
	int16_t pos  = analog_adc_get(POT_LIFT);
	bool    up   = vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	bool    move = up || down;

	ramp_raw(move * vel);
	--data->timeout;
}

bool auto_ramp_isdone(data_t *data) {
	int16_t pos  = analog_adc_get(POT_LIFT);
	bool    up   = vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	
	return !(up || down);
}

/* Do nothing for a given timeout.  */
void auto_noop_init(data_t *data) {
}

void auto_noop_loop(data_t *data) {
	--data->timeout;
}

void auto_noop_isdone(data_t *data) {
	return !data->timeout;
}
