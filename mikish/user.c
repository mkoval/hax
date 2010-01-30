#include "hax.h"

uint8_t kNumAnalogInputs = 2;

enum {
	MTR_DRIVE_F = 0,
	MTR_DRIVE_L,
	MTR_DRIVE_B,
	MTR_DRIVE_R,
	MTR_ARM_L,
	MTR_ARM_R
};

enum {
	OI_R_X = kAnalogSplit,
	OI_R_Y,
	OI_L_Y,
	OI_L_X,
	OI_L_B,
	OI_R_B
};

enum {
	SEN_SCISSOR_L = 0,
	SEN_SCISSOR_R
};


void init(void) {
	puts("Initialization\n");
}

void auton_loop(void) {
	/* there is no autonomous mode */
}
	
void auton_spin(void) {
	/* there is no autonomous mode */
}

void telop_loop(void) {
	int8_t fwrd = analog_oi_get(OI_L_Y);
	int8_t side = analog_oi_get(OI_L_X); 
	int8_t arm  = analog_oi_get(OI_R_Y);
	int8_t spin = analog_oi_get(OI_R_X);

	motor_set(MTR_DRIVE_L, fwrd);
	motor_set(MTR_DRIVE_R, -fwrd + 1);
	motor_set(MTR_DRIVE_F, -side + 1);
	motor_set(MTR_DRIVE_B, side);

	motor_set(MTR_ARM_L, arm);
	motor_set(MTR_ARM_R, -arm + 1);

	puts("F:S :: ");
	_puth(fwrd);
	putc(':');
	_puth(side);
	putc('\n');
}

void telop_spin(void) {
}

