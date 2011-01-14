#include <hax.h>
#include <stdio.h>

#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "util.h"
#include "ru_ir.h"

#include "auton.h"

void auto_deploy_init(state_t const __rom __unused *state, mutable_t __unused *mut) {
}

void auto_deploy_loop(state_t const __rom *state, mutable_t __unused *mut) {
	arm_raw(state->data->pose.vel);
}

/* Drive straight using encoders for velocity control. */
void auto_straight_init(state_t const __rom __unused *state, mutable_t *mut) {
	mut->enc_left  = encoder_get(ENC_L);
	mut->enc_right = encoder_get(ENC_R);
}

void auto_straight_loop(state_t const __rom *state, mutable_t *mut) {
	int32_t left  = encoder_get(ENC_L) - mut->enc_left;
	int32_t right = encoder_get(ENC_R) - mut->enc_right;
	int32_t diff  = left - right;
	int8_t  turn  = PROP(ABS(state->data->move.vel), STRAIGHT_ERROR, ABS(diff));

#if DEBUG
	printf((char *)"delta = %5d, correct = %3d\n", (int)diff, (int)turn);
#endif

	drive_smart(state->data->move.vel, SIGN(diff) * turn);
}

bool auto_straight_isdone(state_t const __rom *state, mutable_t *mut) {
	int32_t left  = encoder_get(ENC_L) - mut->enc_left;
	int32_t right = encoder_get(ENC_R) - mut->enc_right;

#if DEBUG
	printf((char *)"travelled = %5d, target = %5d\n",
	       (int)(ABS(left + right) / 2), (int)(state->data->move.ticks));
#endif

	return ABS(left + right) >= 2 * state->data->move.ticks;
}

void auto_pickup_init(state_t const __rom __unused *state, mutable_t *mut) {
	mut->enc_left  = encoder_get(ENC_L);
	mut->enc_right = encoder_get(ENC_R);
}

void auto_pickup_loop(state_t const __rom *state, mutable_t __unused *mut) {
	drive_smart(state->data->move.vel, 0);
	arm_smart(ARM_SPEEDMAX);
}

bool auto_pickup_isdone(state_t const __rom *state, mutable_t *mut) {
	return auto_arm_isdone(state, mut);
}

/* Drive forward at the desired velocity. */
void auto_drive_init(state_t const __rom __unused *state, mutable_t __unused *mut) {
}

void auto_drive_loop(state_t const __rom *state, mutable_t __unused *mut) {
	drive_smart(state->data->move.vel, 0);
}

/* Turn through a specified angle using encoders as velocity control. */
void auto_turn_init(state_t const __rom __unused *state, mutable_t *mut) {
	/* Store the initial encoder counts for both wheels. */
	mut->enc_left  = encoder_get(ENC_L);
	mut->enc_right = encoder_get(ENC_R);
}

void auto_turn_loop(state_t const __rom *state, mutable_t *mut) {
	int32_t left  = encoder_get(ENC_L) - mut->enc_left;
	int32_t right = encoder_get(ENC_R) - mut->enc_right;
	int32_t diff  = ABS(left - right) / 2 - state->data->move.ticks;
	int8_t  turn  = 50 + PROP(MAX(ABS(state->data->move.vel), 50) - 50,
	                          TURN_ERROR, ABS(diff));

	drive_smart(0, SIGN(state->data->move.vel) * turn);
}

bool auto_turn_isdone(state_t const __rom *state, mutable_t *mut) {
	int32_t left  = encoder_get(ENC_L) - mut->enc_left;
	int32_t right = encoder_get(ENC_R) - mut->enc_right;
	int32_t diff  = ABS(left - right) / 2 - state->data->move.ticks;

	return ABS(diff) < TURN_THRESHOLD;
}

/* Rotate the arm to be in the specified position. */
void auto_arm_init(state_t const __rom __unused *state, mutable_t __unused *mut) {
}

void auto_arm_loop(state_t const __rom *state, mutable_t __unused *mut) {
	int16_t pos  = analog_get(POT_ARM);
	bool    up   = state->data->pose.vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = state->data->pose.vel < 0 && ARM_GT(pos, POT_ARM_LOW);
	bool    move = up || down;

	arm_raw(move * state->data->pose.vel);
}

bool auto_arm_isdone(state_t const __rom *state, mutable_t __unused *mut) {
	int16_t pos  = analog_get(POT_ARM);
	bool    up   = state->data->pose.vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = state->data->pose.vel < 0 && ARM_GT(pos, POT_ARM_LOW);

	return !(up || down);
}

#if defined(ROBOT_KEVIN)

/* Move the ramp into the specific position.  */
void auto_ramp_init(state_t const __rom __unused *state, mutable_t __unused *mut) {
}

void auto_ramp_loop(state_t const __rom *state, mutable_t __unused *mut) {
	int16_t pos  = analog_get(POT_LIFT);
	bool    up   = state->data->pose.vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = state->data->pose.vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	bool    move = up || down;

	ramp_raw(move * state->data->pose.vel, move * state->data->pose.vel);
}

bool auto_ramp_isdone(state_t const __rom *state, mutable_t __unused *mut) {
	int16_t pos  = analog_get(POT_LIFT);
	bool    up   = state->data->pose.vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = state->data->pose.vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);

	return !(up || down);
}

bool auto_ram_isdone(state_t const __rom __unused *state, mutable_t __unused *mut) {
	ir_filter_routine();
	return Get_Rear_IR() < DUMP_DISTANCE_10IN;
}

#elif defined(ROBOT_NITISH)

void auto_ramp_init(state_t const __rom *state, mutable_t *mut) {
}

void auto_ramp_loop(state_t const __rom *state, mutable_t *mut) {
	int8_t  vel   = state->data->pose.vel;
	int16_t left  = analog_get(POT_LIFT_L);
	int16_t right = analog_get(POT_LIFT_R);

	bool move_left  = (vel > 0 && LIFT_L_LT(left, POT_LIFT_L_HIGH))
	               || (vel < 0 && LIFT_L_GT(left, POT_LIFT_L_LOW));
	bool move_right = (vel > 0 && LIFT_R_LT(right, POT_LIFT_R_HIGH))
	               || (vel < 0 && LIFT_R_GT(right, POT_LIFT_R_LOW));

	ramp_raw(move_left * vel, move_right * vel);
}

bool auto_ramp_isdone(state_t const __rom *state, mutable_t *mut) {
	int8_t  vel   = state->data->pose.vel;
	int16_t left  = analog_get(POT_LIFT_L);
	int16_t right = analog_get(POT_LIFT_R);

	bool move_left  = (vel > 0 && LIFT_L_LT(left, POT_LIFT_L_HIGH))
	               || (vel < 0 && LIFT_L_GT(left, POT_LIFT_L_LOW));
	bool move_right = (vel > 0 && LIFT_R_LT(right, POT_LIFT_R_HIGH))
	               || (vel < 0 && LIFT_R_GT(right, POT_LIFT_R_LOW));

	return !(move_left || move_right);
}

bool auto_ram_isdone(state_t const __rom __unused *state, mutable_t __unused *mut) {
	return !digital_get(BUT_B_L) || !digital_get(BUT_B_R);
}
#endif

/* Do nothing for a given timeout.  */
void auto_none_init(state_t const __rom __unused *state, mutable_t __unused *mut) {
}

void auto_none_loop(state_t const __rom __unused *state, mutable_t __unused *mut) {
}

bool auto_none_isdone(state_t const __rom *state, mutable_t *mut) {
#if DEBUG
	printf((char *)"timeout = %d\n", (int)state->data->timeout);
#endif

	return mut->timer >= state->timeout;
}
