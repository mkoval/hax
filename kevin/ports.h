#ifndef PORTS_MIKISH_H_
#define PORTS_MIKISH_H_

#include <hax.h>

/* PWM Motor Outputs */
enum {
	MTR_DRIVE_L = 0,
	MTR_DRIVE_R,
	MTR_LIFT_L,
	MTR_LIFT_R,
	MTR_ARM_A,
	MTR_ARM_B,
    MTR_NUM
};

/* Operator Interface Analog Inputs */
enum {
	OI_R_X = kAnalogSplit,
	OI_R_Y,
	OI_L_Y,
	OI_L_X,
	OI_L_B,
	OI_R_B
};

/* Analog Sensors */
enum {
    POT_ARM = 0,
	POT_LIFT,
    ANA_NUM
};

/* Digital Sensors */
enum {
	/* Rear bump sensors. */
	BUT_B = ANA_NUM,
	BUT_UNUSED1,
	JUMP_CAL_EN,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	SEN_NUM
};

/* Hardware interrupt mappings. */
enum {
    INT_ENC_L1 = 0,
    INT_ENC_L2,
    INT_ENC_R1,
    INT_ENC_R2,
    INT_NUM
};

/* Arbitrary constants used to interface with the encoder API. */
enum {
	ENC_L = 0,
	ENC_R,
	ENC_NUM
};

/* Lift potentiometer, used to measure the orientation of the lift. */
#define POT_LIFT_LOW    850
#define POT_LIFT_HIGH   26

#define LIFT_LT(_p1_, _p2_) ((_p1_) > (_p2_))
#define LIFT_GT(_p1_, _p2_) ((_p1_) < (_p2_))

/* Arm potentiometer, used to measure the orientation of the arm. */
#define POT_ARM_LOW     864
#define POT_ARM_HIGH    169

#define ARM_LT(_p1_, _p2_) ((_p1_) > (_p2_))
#define ARM_GT(_p1_, _p2_) ((_p1_) < (_p2_))

#define ENC_PER_IN      27
#define ENC_PER_DEG     4

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN    18
#define ROB_LENGTH_IN   18
#define ROB_HEIGHT_IN   24
#define ROB_ARM_IN      17

/* Field dimensions; used to calculate drive distances. */
#define FIELD_WIDTH_IN  (12 * 12)
#define FIELD_LENGTH_IN (6  * 12)

/* Calibration routines. */
#define CAL_ENC_DRIVE 1000
#define CAL_ENC_TURN  1000

#endif
