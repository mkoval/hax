#include <hax.h>
#include <stdbool.h>
#include <stdio.h>

#include "auton.h"
#include "encoder.h"
#include "ports.h"
#include "robot.h"

#include "user.h"

/* Physical and electronic robot configuration is specified in ports.h. */
uint8_t kNumAnalogInputs = ANA_NUM;

/* Jumper-enabled calibration modes. */
static CalibrationMode cal_mode = CAL_MODE_NONE;

/* User-override for arm and ramp potentiometers. */
static bool override = false;

/* Forward declare the arrays used by the below macro expansions. */
static state_t const __rom auto_state[];
static data_t              auto_data[];

/* Current state of autonomous mode. Meaningless if in telop mode. */
static state_t const __rom *auto_current;

/* Hack to trick disable the "expression always false" warning. */
static bool kFalse = false;

/* Define transition functions using a helper macro. */
#define AUTO_DONE &auto_state[17]

AUTO_LEAVE(0,  &auto_state[1],  &auto_state[1],  kFalse)
AUTO_LEAVE(1,  AUTO_DONE,       &auto_state[2],  auto_straight_isdone(cur->data))
AUTO_LEAVE(2,  AUTO_DONE,       &auto_state[3],  auto_arm_isdone(cur->data))
AUTO_LEAVE(3,  AUTO_DONE,       &auto_state[4],  auto_ramp_isdone(cur->data))
AUTO_LEAVE(4,  &auto_state[5],  NULL,            kFalse)
AUTO_LEAVE(5,  AUTO_DONE,       &auto_state[6],  auto_ramp_isdone(cur->data))

AUTO_LEAVE(6,  AUTO_DONE,       &auto_state[7],  auto_straight_isdone(cur->data))
AUTO_LEAVE(7,  AUTO_DONE,       &auto_state[8],  auto_turn_isdone(cur->data))
AUTO_LEAVE(8,  AUTO_DONE,       &auto_state[9],  auto_straight_isdone(cur->data))
AUTO_LEAVE(9,  AUTO_DONE,       &auto_state[10], auto_arm_isdone(cur->data))
AUTO_LEAVE(10, &auto_state[11], NULL,            kFalse)
AUTO_LEAVE(11, AUTO_DONE,       &auto_state[12], auto_arm_isdone(cur->data))

AUTO_LEAVE(12, AUTO_DONE,       &auto_state[13], auto_turn_isdone(cur->data))
AUTO_LEAVE(13, AUTO_DONE,       &auto_state[14], auto_straight_isdone(cur->data))
AUTO_LEAVE(14, AUTO_DONE,       &auto_state[15], auto_ramp_isdone(cur->data))
AUTO_LEAVE(15, &auto_state[16], NULL,            kFalse)
AUTO_LEAVE(16, AUTO_DONE,       AUTO_DONE,       auto_ramp_isdone(cur->data))

static data_t auto_data[] = {
	/* Dump preloaded balls. */
	AUTO_ARM(250, 0, kMotorMax),
	AUTO_STRAIGHT(4000, 0, kMotorMax),
	AUTO_ARM(1000, 0, kMotorMin),
	AUTO_RAMP(500, POT_LIFT_HIGH, kMotorMax),
	AUTO_WAIT(1000),
	AUTO_RAMP(500, POT_LIFT_LOW, kMotorMin),

	/* Collect the first three footballs. */
	AUTO_STRAIGHT(500, 60, kMotorMax),
	AUTO_TURN(500, 90, kMotorMax),
	AUTO_STRAIGHT(500, 480, kMotorMax),
	AUTO_ARM(500, POT_ARM_HIGH, kMotorMax),
	AUTO_WAIT(100),
	AUTO_ARM(500, POT_ARM_LOW, kMotorMin),
	
	/* Dump the freshly-harvested balls. */
	AUTO_TURN(500, 90, kMotorMin),
	AUTO_STRAIGHT(500, 0, kMotorMax),
	AUTO_RAMP(250, POT_LIFT_HIGH, kMotorMax),
	AUTO_WAIT(1000),
	AUTO_RAMP(250, POT_LIFT_LOW, kMotorMin),

	/* Do nothing for the remainder of autonomous. */
	AUTO_WAIT(0)
};

static state_t const __rom auto_state[] = {
	/* Dump preloaded balls. */
	{ &auto_data[0],  auto_arm_init,      auto_arm_loop,      AUTO_LOOKUP(0) },
	{ &auto_data[1],  auto_straight_init, auto_straight_loop, AUTO_LOOKUP(1) },
	{ &auto_data[2],  auto_arm_init,      auto_arm_loop,      AUTO_LOOKUP(2) },
	{ &auto_data[3],  auto_ramp_init,     auto_ramp_loop,     AUTO_LOOKUP(3) },
	{ &auto_data[4],  auto_wait_init,     auto_wait_loop,     AUTO_LOOKUP(4) },
	{ &auto_data[5],  auto_ramp_init,     auto_ramp_loop,     AUTO_LOOKUP(5) },

	/* Collect the first three footballs. */
	{ &auto_data[6],  auto_straight_init, auto_straight_loop, NULL },
	{ &auto_data[7],  auto_turn_init,     auto_turn_loop,     NULL },
	{ &auto_data[8],  auto_straight_init, auto_straight_loop, NULL },
	{ &auto_data[9],  auto_arm_init,      auto_arm_loop,      NULL },
	{ &auto_data[10], auto_wait_init,     auto_wait_loop,     NULL },
	{ &auto_data[11], auto_arm_init,      auto_arm_loop,      NULL },
	
	/* Dump the freshly-harvested balls. */
	{ &auto_data[12], auto_turn_init,     auto_turn_loop,     NULL },
	{ &auto_data[13], auto_straight_init, auto_straight_loop, NULL },
	{ &auto_data[14], auto_ramp_init,     auto_ramp_loop,     NULL },
	{ &auto_data[15], auto_wait_init,     auto_wait_loop,     NULL },
	{ &auto_data[16], auto_ramp_init,     auto_ramp_loop,     NULL },

	/* Do nothing for the remainder of autonomous. */
	{ &auto_data[17], NULL,               NULL,               NULL }

};

void init(void) {
	/* Enable the calibration mode specified by the jumpers. */
	cal_mode = (!digital_get(JUMP_CAL_MODE1)     )
	         | (!digital_get(JUMP_CAL_MODE2) << 1)
	         | (!digital_get(JUMP_CAL_MODE3) << 2);

	printf((char *)"[CALIBRATION %d]\r\n", cal_mode);

	/* Initialize autonomous mode. */
	auto_current = &auto_state[0];

	/* Initialize the encoder API; from now on we can use the logical mappings
	 * ENC_L, ENC_R, and ENC_S without worrying about the wiring of the robot.
	 */
	encoder_init(ENC_L, INT_ENC_L1, INT_ENC_L2); /* Left  */
	encoder_init(ENC_R, INT_ENC_R1, INT_ENC_R2); /* Right */
}

void disable_loop(void) {
}

void disable_spin(void) { 
}

void auton_loop(void) {
	state_t const __rom *next;
	uint8_t i;

	/* Start each autonomous slow loop with a clean slate. */
	for (i = 0; i < MTR_NUM; ++i) {
		motor_set(i, 0);
	}

	/* Update the current state using the loop() callback. */
	auto_current->cb_loop(auto_current->data);

	/* We just changed states and need to call the initialization routine for
	 * the new state. Additionally, printf() an alert.
	 */
	next = auto_current->cb_next(auto_current);
	if (auto_current != next) {
		next->cb_init(auto_current->data);
	}
	auto_current = next;
}

void auton_spin(void) {
}

void telop_loop(void) {
	uint16_t left    = analog_oi_get(OI_L_Y);
	uint16_t right   = analog_oi_get(OI_R_Y);
	uint16_t arm     = analog_oi_get(OI_L_B);
	uint16_t ramp    = analog_oi_get(OI_R_B);
	bool     done    = false;

	switch (cal_mode) {
	/* Calibrate the ENC_PER_IN constant. */
	case CAL_MODE_DRIVE: {
		int32_t dist = drive_straight(kMotorMax);
		done = dist > CAL_ENC_DRIVE;
		break;
	}

	/* Calibrate the ENC_PER_DEG constant. */
	case CAL_MODE_TURN: {
		done = drive_turn(90);
		break;
	}
	
	/* Calibrate the POT_ARM_LOW and POT_ARM_HIGH constants. */
	case CAL_MODE_PRINT:
		printf((char *)"ARM %4d   LIFT %4d   ENCL %5d   ENCR %5d\n\r",
		       (int)analog_adc_get(POT_ARM),
		       (int)analog_adc_get(POT_LIFT),
		       (int)encoder_get(ENC_L),
		       (int)encoder_get(ENC_R));
		/* Fall through to allow normal telop control. */
	
	/* Normal user-controlled telop mode. */
	default:
		/* Disable ramp and arm potientiometer checks. */
		if (override) {
			drive_raw(left, right);
			arm_raw(arm);
			ramp_raw(ramp);
		}
		/* Prevent dangerous ramp and arm movement in software. */
		else {
			drive_raw(left, right);
			arm_smart(arm);
			ramp_raw(ramp);
		}
	}

	/* End calibration mode when the routine is complete. */
	cal_mode *= !done;
}

void telop_spin(void) {
}
