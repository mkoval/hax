#ifndef PORTS_COMP2010_H_
#define PORTS_COMP2010_H_

#include <hax.h>

#if defined(ARCH_PIC)
/* Operator interface port mappings are not robot specific. */
enum {
	OI_R_X = kAnalogSplit,
	OI_R_Y,
	OI_L_Y,
	OI_L_X,
	OI_L_B,
	OI_R_B
};
#elif defined(ARCH_CORTEX)
enum {
	OI_STICK_L_X,
	OI_STICK_L_Y,
	OI_STICK_R_X,
	OI_STICK_R_Y,
	OI_BUT_L_U,
	OI_BUT_L_D,
	OI_BUT_L_L,
	OI_BUT_L_R,
	OI_BUT_R_U,
	OI_BUT_R_D,
	OI_BUT_R_L,
	OI_BUT_R_R,
	OI_TRIG_L_U,
	OI_TRIG_L_D,
	OI_TRIG_R_U,
	OI_TRIG_R_D
};
#endif

#if defined(ROBOT_KEVIN)
/* PWM Motor Outputs */
enum {
	MTR_DRIVE_L = 0,
	MTR_DRIVE_R,
	MTR_LIFT_L,
	MTR_LIFT_R,
	MTR_ARM_LT,
	MTR_ARM_LB,
	MTR_ARM_RT,
	MTR_ARM_RB,
	MTR_NUM
};

#if defined(ARCH_PIC)
enum {
	INT_ENC_L1 = 17,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};
enum {
	POT_ARM = 0,
	POT_LIFT,
	IR_FRONT_H = 7, /* High */
	IR_SIDE_B,
	IR_REAR,
	IR_FRONT_L,     /* Low */
	ANA_NUM
};
enum {
	BUT_B = ANA_NUM,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
}

#define ENC_PER_IN      100
#define ENC_PER_DEG     15

#elif defined(ARCH_CORTEX)
enum {
	INT_ENC_L1 = 17,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};
enum {
	JUMP_CAL_MODE1 = INT_NUM,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
};
enum {
	POT_ARM = 0,
	POT_LIFT,
	IR_REAR,
	ANA_NUM
};

#define ENC_PER_IN      150
#define ENC_PER_DEG     15

#endif

enum {
	ENC_L = 0,
	ENC_R,
	ENC_NUM
};

/* Where to stop to successfully score. */
#define DUMP_DISTANCE_10IN 90

/* Lift potentiometer, used to measure the orientation of the lift. */
#define POT_LIFT_LOW   75
#define POT_LIFT_HIGH  945

#define LIFT_LT(_p1_, _p2_) ((_p1_) < (_p2_))
#define LIFT_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Arm potentiometer, used to measure the orientation of the arm. */
#define POT_ARM_LOW     850
#define POT_ARM_HIGH    200

#define ARM_LT(_p1_, _p2_) ((_p1_) > (_p2_))
#define ARM_GT(_p1_, _p2_) ((_p1_) < (_p2_))

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN    18
#define ROB_LENGTH_IN   18
#define ROB_HEIGHT_IN   24
#define ROB_ARM_IN      17

#define ARM_SPEEDMAX    85
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

#if defined(ARCH_PIC)
/* Digital and Analog IO */
enum {
	POT_ARM = 0,
	POT_LIFT_L,
	POT_LIFT_R,
	ANA_NUM
};
enum {
	BUT_B_R = ANA_NUM,
	BUT_B_L,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
};

/* Encoder Interrupts */
enum {
	INT_ENC_L1 = 17,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};

#define ENC_PER_IN      100
#define ENC_PER_DEG     15

#elif defined(ARCH_CORTEX)
/* Encoder Interrupts */
enum {
	INT_ENC_L1 = 0,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};

/* Digital Inputs */
enum {
	BUT_B_L = INT_NUM,
	BUT_B_R,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
};

/* Analog Inputs */
enum {
	POT_ARM = 0,
	POT_LIFT_L,
	POT_LIFT_R,
	ANA_NUM
};

#define ENC_PER_IN      150
#define ENC_PER_DEG     15

#define ARM_SPEEDMAX    127
#endif

enum {
	ENC_L = 0,
	ENC_R,
	ENC_NUM
};

/* Lift potentiometer, used to measure the orientation of the lift. */
#define POT_LIFT_L_LOW    430
#define POT_LIFT_L_HIGH   300
#define POT_LIFT_R_LOW    166
#define POT_LIFT_R_HIGH   276

#define LIFT_L_LT(_p1_, _p2_) ((_p1_) > (_p2_))
#define LIFT_L_GT(_p1_, _p2_) ((_p1_) < (_p2_))
#define LIFT_R_LT(_p1_, _p2_) ((_p1_) < (_p2_))
#define LIFT_R_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Arm potentiometer, used to measure the orientation of the arm. */
#define POT_ARM_LOW     130
#define POT_ARM_HIGH    860

#define ARM_LT(_p1_, _p2_) ((_p1_) < (_p2_))
#define ARM_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN    18
#define ROB_LENGTH_IN   24
#define ROB_HEIGHT_IN   18
#define ROB_ARM_IN      17

#define ARM_SPEEDMAX    127

#endif
#endif
