#ifndef PORTS_COMP2010_H_
#define PORTS_COMP2010_H_

#include <hax.h>

/* Operator interface port mappings are not robot specific. */
enum {
	OI_R_X = kAnalogSplit,
	OI_R_Y,
	OI_L_Y,
	OI_L_X,
	OI_L_B,
	OI_R_B
};

#if defined(ROBOT_KEVIN)
/* PWM Motor Outputs */
enum {
	MTR_DRIVE_L = 0,
	MTR_DRIVE_R,
	MTR_LIFT_L,
	MTR_LIFT_R,
	MTR_ARM_A1,
	MTR_ARM_B1,
	MTR_ARM_A2,
	MTR_ARM_B2,
    MTR_NUM
};


/* Analog Sensors */
enum {
    POT_ARM = 0,
	POT_LIFT,
	IR_FRONT_H = 7, /* High */
	IR_SIDE_F,
	IR_SIDE_B,
	IR_REAR,
	IR_FRONT_L,     /* Low */
    ANA_NUM
};

/* Digital Sensors */
enum {
	BUT_B = ANA_NUM,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
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

/* Where to stop to successfully score. */
#define DUMP_DISTANCE_10IN 55

/* Lift potentiometer, used to measure the orientation of the lift. */
#define POT_LIFT_LOW    275
#define POT_LIFT_HIGH   1000

#define LIFT_LT(_p1_, _p2_) ((_p1_) < (_p2_))
#define LIFT_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Arm potentiometer, used to measure the orientation of the arm. */
#define POT_ARM_LOW     850
#define POT_ARM_HIGH    220

#define ARM_LT(_p1_, _p2_) ((_p1_) > (_p2_))
#define ARM_GT(_p1_, _p2_) ((_p1_) < (_p2_))

#define ENC_PER_IN      100
#define ENC_PER_DEG     15

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN    18
#define ROB_LENGTH_IN   18
#define ROB_HEIGHT_IN   24
#define ROB_ARM_IN      17

#define LIFT_DEPLOY_VEL 50

#elif defined(ROBOT_NITISH)

enum {
	MTR_LIFT_L,
	MTR_LIFT_R,
	MTR_DRIVE_L1,
	MTR_DRIVE_L2,
	MTR_DRIVE_R1,
	MTR_DRIVE_R2,
	MTR_ARM_L,
	MTR_ARM_R,
    MTR_NUM
};

/* Analog Sensors */
enum {
    POT_ARM = 0,
	POT_LIFT_L,
	POT_LIFT_R,
    ANA_NUM
};

/* Digital Sensors */
enum {
	BUT_B_R = ANA_NUM,
	BUT_B_L,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
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
#define POT_LIFT_L_LOW    450
#define POT_LIFT_L_HIGH   250
#define POT_LIFT_R_LOW    190
#define POT_LIFT_R_HIGH   320

#define LIFT_L_LT(_p1_, _p2_) ((_p1_) > (_p2_))
#define LIFT_L_GT(_p1_, _p2_) ((_p1_) < (_p2_))
#define LIFT_R_LT(_p1_, _p2_) ((_p1_) < (_p2_))
#define LIFT_R_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Arm potentiometer, used to measure the orientation of the arm. */
#define POT_ARM_LOW     130
#define POT_ARM_HIGH    860

#define ARM_LT(_p1_, _p2_) ((_p1_) < (_p2_))
#define ARM_GT(_p1_, _p2_) ((_p1_) > (_p2_))

#define ENC_PER_IN      100
#define ENC_PER_DEG     15

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN    18
#define ROB_LENGTH_IN   24
#define ROB_HEIGHT_IN   18
#define ROB_ARM_IN      17

#endif

/* Calibration routines. */
#define CAL_ENC_DRIVE 1000
#define CAL_ENC_TURN  1000

#endif
