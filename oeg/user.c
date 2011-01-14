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
	ix2 = (int)(IX_OI_BUTTON(1, 7, OI_B_UP));
	printf("%d\n", ix2);

#if 0
	int8_t oi_arm = oi_button_get();

	int8_t oi_right = oi_group_get(OI_JOY_R_Y(1));
	int8_t oi_left  = oi_group_get(OI_JOY_L_Y(1));

	motor_set(MOTOR_DRIVE_R1, -oi_right);
	motor_set(MOTOR_DRIVE_R2, -oi_right);
	motor_set(MOTOR_DRIVE_L1, +oi_left);
	motor_set(MOTOR_DRIVE_L2, +oi_left);

	motor_set(MOTOR_ARM_R1, -oi_arm);
	motor_set(MOTOR_ARM_R2, -oi_arm);
	motor_set(MOTOR_ARM_L1, +oi_arm);
	motor_set(MOTOR_ARM_L2, +oi_arm);
#endif
}

void telop_spin(void) {
}

void disable_loop(void) {
}

void disable_spin(void) {
}
