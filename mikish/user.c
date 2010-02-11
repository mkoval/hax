#include "hax.h"
#include <stdio.h>

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
	_puts("Initialization\n");
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

#define ABS(x)  ( (x) > 0 ? (x) : -(x) )
#define MAX(x,y) ( (x) > (y) ? (x) : (y) )
#define AMAX4(a,b,c,d) MAX(MAX(ABS(a),ABS(b)),MAX(ABS(c),ABS(d)))

static int16_t max4(int16_t a, int16_t b, int16_t c, int16_t d) {
	if ( a >= b ) {
		if ( a >= c ) {
			if ( a >= d ) {
				return a;
			} else {
				return d;
			}
		} else {
			if ( c >= d ) {
				return c;
			} else {
				return d;
			}
		}
	} else {
		if ( b >= c ) {
			if ( b >= d ) {
				return b;
			} else {
				return d;
			}
		} else {
			if ( c >= d ) {
				return c;
			} else {
				return d;
			}
		}
	}
}

/*
x = side to side (+ = right)
y = forwards & backwards (+ = forwards)
z = spin (+ = clockwise)
*/
void omni(int8_t x, int8_t y, int8_t z) {
	int16_t F = (int16_t) x + z;
	int16_t R = (int16_t)-y + z;
	int16_t B = (int16_t)-x + z;
	int16_t L = (int16_t) y + z;

	int16_t max = max4(ABS(L),ABS(R),ABS(B),ABS(F));
	printf("F:%4d R:%4d B:%4d L:%4d max:%4d :: ",F,R,B,L,max);
	if ( max > kMotorMax ) {
		/* scale */
		L = L * kMotorMax / max;
		R = R * kMotorMax / max;
		B = B * kMotorMax / max;
		F = F * kMotorMax / max;
		printf("F:%4d R:%4d B:%4d L:%4d\n",F,R,B,L);	
	} else { 
		printf("F:%4d R:%4d B:%4d L:%4d\n",F,R,B,L);
	}
	motor_set(MTR_DRIVE_L, L);
	motor_set(MTR_DRIVE_R, R);
	motor_set(MTR_DRIVE_B, B);
	motor_set(MTR_DRIVE_F, F);
}

void telop_loop(void) {
	int8_t fwrd = analog_oi_get(OI_L_Y);
	int8_t side = analog_oi_get(OI_L_X); 
	int8_t arm  = button(analog_oi_get(OI_L_B)) * kMotorMax;
	int8_t spin = analog_oi_get(OI_R_X);
	int8_t lift = button(analog_oi_get(OI_R_B)) * kMotorMax;

	//printf("F:%d R:%d B:%d L:%d\n",side,-fwrd,-side,fwrd);

	//motor_set(MTR_DRIVE_F,  side);
	//motor_set(MTR_DRIVE_R, -fwrd);
	//motor_set(MTR_DRIVE_B, -side);
	//motor_set(MTR_DRIVE_L,  fwrd);

	omni(-side, fwrd, -spin);

	motor_set(MTR_ARM_L, arm);
	motor_set(MTR_ARM_R, -arm);

	motor_set(MTR_SCISSOR_L, lift);
	motor_set(MTR_SCISSOR_R, -lift);
}

void telop_spin(void) {
}

