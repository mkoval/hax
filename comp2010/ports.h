#ifndef PORTS_COMP2010_H_
#define PORTS_COMP2010_H_

#include <hax.h>

enum {
	OI_R_X = OI_JOY_R_X(1),
	OI_R_Y = OI_JOY_R_Y(1),
	OI_L_X = OI_JOY_L_X(1),
	OI_L_Y = OI_JOY_L_Y(1),

	OI_L_B = OI_ROCKER_L(1),
	OI_R_B = OI_ROCKER_R(1),

	OI_BUT_L_U = OI_TRIGGER_L(1, OI_B_UP),
	OI_BUT_L_D = OI_TRIGGER_L(1, OI_B_DN),

	OI_BUT_R_U = OI_TRIGGER_R(1, OI_B_UP),
	OI_BUT_R_D = OI_TRIGGER_R(1, OI_B_DN),
};

#if defined(ROBOT_NITISH)
enum nitish_motor_e {
	MTR_LIFT_L,
	MTR_LIFT_R,
	MTR_DRIVE_L1,
	MTR_DRIVE_L2,
	MTR_DRIVE_R1,
	MTR_DRIVE_R2,
	MTR_ARM_L,
	MTR_ARM_R,
	MTR_LAST
};
#elif defined(ROBOT_KEVIN)
enum kevin_motor_e {
	MTR_DRIVE_L = IX_MOTOR(1),
	MTR_DRIVE_R,
	MTR_LIFT_L,
	MTR_LIFT_R,
	MTR_ARM_LT,
	MTR_ARM_LB,
	MTR_ARM_RT,
	MTR_ARM_RB,
	MTR_LAST
};
#else
# error "Bad ROBOT_*"
#endif


#define MTR_NUM ((MTR_LAST) - IX_MOTOR(1) + 1)

#if defined(ROBOT_KEVIN)
/* PWM Motor Outputs */

# if defined(ARCH_PIC)
enum kevin_pic_int_e {
	INT_ENC_L1 = IX_INTERRUPT(1),
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};
enum kevin_pic_ana_e {
	POT_ARM = IX_ANALOG(1),
	POT_LIFT,
	IR_FRONT_H = IX_ANALOG(8), /* High */
	IR_SIDE_B,
	IR_REAR,
	IR_FRONT_L,     /* Low */
	ANA_LAST
};
#  define ANA_NUM IX_ANALOG_INV(ANA_LAST)
enum kevin_pic_dig_e {
	BUT_B = IX_DIGITAL(ANA_NUM + 1),
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
}

#  define ENC_PER_IN      100
#  define ENC_PER_DEG     15

# elif defined(ARCH_CORTEX)
enum kevin_cortex_int_e {
	INT_ENC_L1 = IX_INTERRUPT(1),
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};
enum kevin_cortex_dig_e {
	JUMP_CAL_MODE1 = INT_NUM,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
};
enum kevin_cortex_ana_e {
	POT_ARM = IX_ANALOG(1),
	POT_LIFT,
	IR_REAR,
	ANA_NUM
};

#  define ENC_PER_IN      150
#  define ENC_PER_DEG     15

# endif /* ARCH_CORTEX */

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

/* !ROBOT_KEVIN */
#elif defined(ROBOT_NITISH)


# if defined(ARCH_PIC)
/* Digital and Analog IO */
enum nitish_pic_ana_e {
	POT_ARM = 0,
	POT_LIFT_L,
	POT_LIFT_R,
	ANA_NUM
};
enum nitish_pic_dig_e {
	BUT_B_R = ANA_NUM,
	BUT_B_L,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
};

/* Encoder Interrupts */
enum nitish_pic_int_e {
	INT_ENC_L1 = 17,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};

#  define ENC_PER_IN      100
#  define ENC_PER_DEG     15

# elif defined(ARCH_CORTEX)
/* Encoder Interrupts */
enum nitish_cortex_int_e {
	INT_ENC_L1 = 0,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_NUM
};

/* Digital Inputs */
enum nitish_cortex_dig_e {
	BUT_B_L = INT_NUM,
	BUT_B_R,
	JUMP_CAL_MODE1,
	JUMP_CAL_MODE2,
	JUMP_CAL_MODE3,
	SEN_NUM
};

/* Analog Inputs */
enum nitish_cortex_ana_e {
	POT_ARM = 0,
	POT_LIFT_L,
	POT_LIFT_R,
	ANA_NUM
};

#  define ENC_PER_IN      150
#  define ENC_PER_DEG     15

#  define ARM_SPEEDMAX    127
# endif /* ARCH_CORTEX */

/* Lift potentiometer, used to measure the orientation of the lift. */
# define POT_LIFT_L_LOW    430
# define POT_LIFT_L_HIGH   300
# define POT_LIFT_R_LOW    166
# define POT_LIFT_R_HIGH   276

# define LIFT_L_LT(_p1_, _p2_) ((_p1_) > (_p2_))
# define LIFT_L_GT(_p1_, _p2_) ((_p1_) < (_p2_))
# define LIFT_R_LT(_p1_, _p2_) ((_p1_) < (_p2_))
# define LIFT_R_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Arm potentiometer, used to measure the orientation of the arm. */
# define POT_ARM_LOW     130
# define POT_ARM_HIGH    860

# define ARM_LT(_p1_, _p2_) ((_p1_) < (_p2_))
# define ARM_GT(_p1_, _p2_) ((_p1_) > (_p2_))

/* Robot dimensions, used to refine drive distances. */
# define ROB_WIDTH_IN    18
# define ROB_LENGTH_IN   24
# define ROB_HEIGHT_IN   18
# define ROB_ARM_IN      17

# define ARM_SPEEDMAX    127

#endif

#endif /* PORTS_COMP2010_H_ */
