#include <hax.h>
#include <stdbool.h>
#include <stdio.h>

#include "auton.h"
#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "state.h"

#include "ru_ir.h"

#include "user.h"

/* Physical and electronic robot configuration is specified in ports.h. */
uint8_t const kNumAnalogInputs = ANA_NUM;

/* User-override for arm and ramp potentiometers. */
bool override = false;

void init(void) {
	/* Initialize autonomous mode. */
	auto_current->cb_init(auto_current, &auto_mutable);
	_puts("[STATE ");
	_puts(auto_current->name);
	_puts("]\n\r");

#if defined(ROBOT_NITISH)
	pin_set_io(BUT_B_L, kInput);
	pin_set_io(BUT_B_R, kInput);
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
	uint8_t i;

	/* Remove outliers from the IR distance sensor data. */
#if defined(ROBOT_KEVIN)
	IR_Filter_Routine();
#endif

	/* Start each autonomous slow loop with a clean slate. */
	for (i = 0; i < MTR_NUM; ++i) {
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

		_puts("[STATE ");
		_puts(auto_current->name);
		_puts("]\n\r");

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

void telop_loop(void) {
#if defined(ARCH_PIC)
	uint16_t left  = analog_oi_get(OI_L_Y);
	uint16_t right = analog_oi_get(OI_R_Y);
	uint16_t arm   = analog_oi_get(OI_L_B);
	uint16_t ramp  = analog_oi_get(OI_R_B);
#elif defined(ARCH_CORTEX)
	uint16_t left  = CONSTRAIN(analog_oi_get(OI_STICK_L_Y), -127, 127);
	uint16_t right = CONSTRAIN(analog_oi_get(OI_STICK_R_Y), -127, 127);
	uint16_t arm   = 127*analog_oi_get(OI_TRIG_L_U) + -127*analog_oi_get(OI_TRIG_L_D);
	uint16_t ramp  = 127*analog_oi_get(OI_TRIG_R_U) + -127*analog_oi_get(OI_TRIG_R_D);

	/* Override potentiometer motor limits. */
	bool override_set   = analog_oi_get(OI_BUT_L_D) && analog_oi_get(OI_BUT_R_D);
	bool override_reset = analog_oi_get(OI_BUT_L_U) && analog_oi_get(OI_BUT_R_U);
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
	IR_Filter_Routine();
	printf((char *)"ARM %4d  LIFT %4d  ENCL %5d  ENCR %5d BACKIR %5d\n\r",
		   (int)analog_adc_get(POT_ARM),
		   (int)analog_adc_get(POT_LIFT),
		   (int)encoder_get(ENC_L),
		   (int)encoder_get(ENC_R),
		   (int)Get_Rear_IR());
#elif defined(ROBOT_NITISH)
	printf((char *)"ARM %4d  LIFTL %4d  LIFTR %4d  ENCL %5d  ENCR %5d\n\r",
		   (int)analog_adc_get(POT_ARM),
		   (int)analog_adc_get(POT_LIFT_L),
		   (int)analog_adc_get(POT_LIFT_R),
		   (int)encoder_get(ENC_L),
		   (int)encoder_get(ENC_R));
#endif
}

void telop_spin(void) {
}
