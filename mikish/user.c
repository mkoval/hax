#include "hax.h"

uint8_t kNumAnalogInputs = 2;

enum {
	MTR_DRIVE_F = 0,
	MTR_DRIVE_L,
	MTR_DRIVE_B,
	MTR_DRIVE_R,
	MTR_ARM_L,
	MTR_ARM_R,
	MTR_SCISSOR_L,
	MTR_SCISSOR_R
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

int8_t button(int8_t v) {
	if ( v > 50 )
		return 1;
	else if ( v < -50 )
		return -1;
	else
		return 0;
}

#define ABS(x)  ( x > 0 ? x : -x )
#define MAX(x,y) ( x > y ? x : y )
#define AMAX4(a,b,c,d) MAX(MAX(ABS(a),ABS(b)),MAX(ABS(c),ABS(d)))

#if 0
void omni(int8_t x, int8_t y, int8_t z) {
	/*
	x : strafing l & r 
	y : fwd & back motion
	z : spin
	*/

	int L = (int)x + z;
	int R = (int)x - z;
	int B = (int)y + z;
	int F = (int)y - z;

	int max = AMAX4(L,R,B,F);

	if ( max > kMotorMax ) {
		/* scale */
		float div = fabs( (float)max / kMotorMax );
		L /= div;
		R /= div;
		B /= div;
		F /= div;

	}

	motor_set(MTR_DRIVE_L,L);
	motor_set(MTR_DRIVE_R,-R);
	motor_set(MTR_DRIVE_B,B);
	motor_set(MTR_DRIVE_F,-F);

}
#endif

void telop_loop(void) {
	int8_t fwrd = analog_oi_get(OI_L_Y);
	int8_t side = analog_oi_get(OI_L_X); 
	int8_t arm  = button(analog_oi_get(OI_L_B)) * kMotorMax;
	/* int8_t spin = analog_oi_get(OI_R_X);*/
	int8_t lift = analog_oi_get(OI_R_B);

	motor_set(MTR_DRIVE_L, fwrd);
	motor_set(MTR_DRIVE_R, -fwrd);
	motor_set(MTR_DRIVE_F, -side);
	motor_set(MTR_DRIVE_B, side);
	
	/*
	omni(side,frwd,spin)
	*/

	motor_set(MTR_ARM_L, arm);
	motor_set(MTR_ARM_R, -arm);

	/* Lift the scissors. */
	motor_set(MTR_SCISSOR_L, lift);
	motor_set(MTR_SCISSOR_R, -lift);

	puts("F:S :: ");
	_puth(fwrd);
	putc(':');
	_puth(side);
	putc('\n');
}

void telop_spin(void) {
}

