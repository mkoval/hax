#include <hax.h>
#include <stdio.h>
#include "user.h"

void init(void) {
}

void auton_loop(void) {
}

void auton_spin(void) {
}

void telop_loop(void) {
	int8_t oi_left  = oi_group_get(OI_JOY_L_Y(1));
	int8_t oi_right = oi_group_get(OI_JOY_R_X(1)); // bug
	int8_t oi_arm   = oi_group_get(OI_ROCKER_L(1));
	int8_t oi_suck  = oi_group_get(OI_ROCKER_R(1));

	printf("left stick: %3d\n", oi_left);
	motor_set(MOTOR_DRIVE_L, +oi_left);
	motor_set(MOTOR_DRIVE_R, -oi_right);
	motor_set(MOTOR_ARM_L1,  +oi_arm);
	motor_set(MOTOR_ARM_R2,  +oi_arm);
	motor_set(MOTOR_ARM_R1,  -oi_arm);
	motor_set(MOTOR_ARM_R2,  -oi_arm);
	motor_set(MOTOR_SUCKER,   oi_suck);
}

void telop_spin(void) {
}

void disable_loop(void) {
}

void disable_spin(void) {
}
