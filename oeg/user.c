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
	int8_t oi_left = oi_group_get(OI_JOY_L_Y(1));
	printf("set...   ");
	motor_set(IX_MOTOR(1), oi_left);
}

void telop_spin(void) {
}

void disable_loop(void) {
}

void disable_spin(void) {
}
