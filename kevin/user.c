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

/* Autonomous queue; used to build a specific autonomous mode from the building
 * blocks provided in the state machine defined in auton.c.
 */
static AutonQueue queue = { 0 };

/* Jumper-enabled calibration modes. */
static CalibrationMode cal_mode = CAL_MODE_NONE;

/* User-override for arm and ramp potentiometers. */
static bool override = false;

void init(void) {
	/* Disable autonomous in calibration mode. */
	cal_mode = !digital_get(JUMP_CAL_EN);

	if (!cal_mode) {
		/* Deploy the robot and dump preloaded balls. */
		auton_enqueue(&queue, AUTO_DEPLOY, 8);
		auton_enqueue(&queue, AUTO_REVRAM, NONE);
		auton_enqueue(&queue, AUTO_ARM,    kMotorMin);
		auton_enqueue(&queue, AUTO_RAMP,   kMotorMax);
		auton_enqueue(&queue, AUTO_WAIT,   150);
		auton_enqueue(&queue, AUTO_RAMP,   kMotorMin);

		/* Drive to the line and collect the first three footballs. */
		auton_enqueue(&queue, AUTO_DRIVE,  240 - ROB_LENGTH_IN * 10);
		auton_enqueue(&queue, AUTO_TURN,   90);
		auton_enqueue(&queue, AUTO_DRIVE,  240);
		auton_enqueue(&queue, AUTO_ARM,    kMotorMax);
		auton_enqueue(&queue, AUTO_WAIT,   50);
		auton_enqueue(&queue, AUTO_ARM,    kMotorMin);
	
		/* Dump the previously collected footballs. */
		auton_enqueue(&queue, AUTO_TURN,   -90);
		auton_enqueue(&queue, AUTO_REVRAM, NONE);
		auton_enqueue(&queue, AUTO_RAMP,   kMotorMax);
		auton_enqueue(&queue, AUTO_WAIT,   150);
		auton_enqueue(&queue, AUTO_RAMP,   kMotorMin);

		auton_enqueue(&queue, AUTO_DONE,   NONE);

		_puts("[CALIBRATION OFF]\r\n");
	} else {
		/* Use the other jumpers to select the correct calibration mode. */
		cal_mode += (!digital_get(JUMP_CAL_MODE1)) << 1;
		cal_mode += (!digital_get(JUMP_CAL_MODE2)) << 2;

		printf((char *)"[CALIBRATION %d]\r\n", cal_mode);
	}

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
	uint8_t i;

	for (i = 0; i < MTR_NUM; ++i) {
		motor_set(i, 0);
	}

	auton_do(&queue);
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
	case CAL_MODE_DRIVE:
		done = drive_straight(kMotorMax) <= CAL_ENC_DRIVE;
		break;

	/* Calibrate the ENC_PER_DEG constant. */
	case CAL_MODE_TURN:
		done = drive_turn(kMotorMax) <= CAL_ENC_TURN;
		break;
	
	/* Calibrate the POT_ARM_LOW and POT_ARM_HIGH constants. */
	case CAL_MODE_PRINT:
		printf((char *)"ARM %4d   LIFT %4d   ENCL %5d   ENCR %5d\n\r",
		       (int)analog_adc_get(POT_ARM),
		       (int)analog_adc_get(POT_LIFT),
		       (int)encoder_get(ENC_L),
		       (int)encoder_get(ENC_R));
		break;
	
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
