#include <hax.h>
#include <stdio.h>
#include "user.h"

enum {
	MOTOR_DRIVE_R1 = IX_MOTOR(2),
	MOTOR_DRIVE_R2 = IX_MOTOR(3),
	MOTOR_DRIVE_L1 = IX_MOTOR(4),
	MOTOR_DRIVE_L2 = IX_MOTOR(5),
	MOTOR_ARM_R1   = IX_MOTOR(6),
	MOTOR_ARM_R2   = IX_MOTOR(7),
	MOTOR_ARM_L1   = IX_MOTOR(8),
	MOTOR_ARM_L2   = IX_MOTOR(9)
};

void init(void) {
	printf("INIT\n");
}

void auton_loop(void) {
	printf("AUTON\n");
}

void auton_spin(void) {
	/* Not recomended to print constantly */
}

int ix2 = 2;

void telop_loop(void) {
	int8_t oi_right = oi_group_get(OI_JOY_R_Y(1));
	int8_t oi_left  = oi_group_get(OI_JOY_L_Y(1));
	int8_t oi_arm   = oi_rocker_get(OI_ROCKER_L(1));

	printf("left, up   = %d\n", (int)oi_button_get(IX_OI_BUTTON(1, 5, OI_B_UP)));
	printf("left, down = %d\n", (int)oi_button_get(IX_OI_BUTTON(1, 5, OI_B_DN)));
	motor_set(IX_MOTOR(2), oi_arm);
}

void telop_spin(void) {
}

void disable_loop(void) {
}

void disable_spin(void) {
}
