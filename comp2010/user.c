#include <stdbool.h>
#include <stdio.h>

#include <hax.h>
#include "auton.h"
#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "state.h"

#include "ru_ir.h"

#include "misc.h"
#include "user.h"

/* User-override for arm and ramp potentiometers. */
static bool override = false;

void init(void) {
	/* Initialize autonomous mode. */
	auto_current->cb_init(auto_current, &auto_mutable);
	fputs("[STATE ", stdout);
	fputs(auto_current->name, stdout);
	fputs("]\n", stdout);

#if defined(ROBOT_NITISH)
	digital_setup(BUT_B_L, DIGITAL_IN);
	digital_setup(BUT_B_R, DIGITAL_IN);
#endif

	override = false;

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
	state_t const __rom *next = auto_current;
	index_t i;

	/* Remove outliers from the IR distance sensor data. */
#if defined(ROBOT_KEVIN)
	ir_filter_routine();
#endif

	/* Start each autonomous slow loop with a clean slate. */
	for (i = IX_MOTOR(1); i < MTR_NUM; ++i) {
		motor_set(i, 0);
	}

	/* Update the current state. */
	auto_current->cb_loop(auto_current, &auto_mutable);

	/* We just changed states and need to call the initialization routine for
	 * the new state. Additionally, printf() an alert.
	 */
	next = auto_current->cb_next(auto_current, &auto_mutable);

	/* Use the state-specific trasition function to get the next state. */
	if (auto_current != next) {
		uint8_t *bytes = (uint8_t *)&auto_mutable;
		uint8_t i;

		auto_current = next;

		fputs("[STATE ", stdout);
		fputs(auto_current->name, stdout);
		fputs("]\n", stdout);

		/* Perform auto_mutable initialization for the new state; replacing the
		 * non-functioning memset() implementation on the PIC.
		 */
		for (i = 0; i < sizeof(mutable_t); ++i) {
			bytes[i] = 0;
		}
		next->cb_init(auto_current, &auto_mutable);
	} else {
		++auto_mutable.timer;
	}
}

void auton_spin(void) {
	encoder_update();
}

void telop_loop(void)
{
	int8_t left  = oi_group_get(OI_L_Y);
	int8_t right = oi_group_get(OI_R_Y);
	int8_t arm   = ARM_SPEEDMAX * oi_rocker_get(OI_L_B);
	int8_t ramp  = 127 * oi_rocker_get(OI_R_B);
#if defined(ARCH_PIC)
#elif defined(ARCH_CORTEX)
	/* Override potentiometer motor limits. */
	bool override_set   = oi_button_get(OI_BUT_L_D) && oi_button_get(OI_BUT_R_D);
	bool override_reset = oi_button_get(OI_BUT_L_U) && oi_button_get(OI_BUT_R_U);
	override = override_set || (override && !override_reset);

	printf("OVERRIDE %d  ", override);
#endif

	/* Disable ramp and arm potientiometer checks. */
	if (override) {
		drive_raw(left, right);
		arm_raw(arm);
		ramp_raw(ramp, ramp);
	}
	/* Prevent dangerous ramp and arm movement in software. */
	else {
		drive_raw(left, right);
		arm_smart(arm);
		ramp_smart(ramp);
	}

#if defined(ROBOT_KEVIN)
	ir_filter_routine();
	printf((char *)"ARM %4d  LIFT %4d  ENCL %5d  ENCR %5d BACKIR %5d\n",
		   (int)analog_get(POT_ARM),
		   (int)analog_get(POT_LIFT),
		   (int)encoder_get(ENC_L),
		   (int)encoder_get(ENC_R),
		   (int)Get_Rear_IR());
#elif defined(ROBOT_NITISH)
	printf((char *)"ARM %4d  LIFTL %4d  LIFTR %4d  ENCL %5d  ENCR %5d\n",
		   (int)analog_get(POT_ARM),
		   (int)analog_get(POT_LIFT_L),
		   (int)analog_get(POT_LIFT_R),
		   (int)encoder_get(ENC_L),
		   (int)encoder_get(ENC_R));
#endif
}

void telop_spin(void) {
}
