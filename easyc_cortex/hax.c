#include <hax.h>
#include <stdint.h>

#include "easyclib/API.h"

void IO_Initialization(void) {
    /* Enable VexNET control in the EasyC library. */
	SetCompetitionMode(-1, -1);

    /* ??? */
	DefineControllerIO(1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0);
}

void setup_1(void) {
    /* Handled by the EasyC library. */
}

void setup_2(void) {
    /* Handled by the EasyC library. */
}

void spin(void) {
}

void loop_1(void) {
    /* Handled by the EasyC library. */
}

void loop_2(void) {
    /* Handled by the EasyC library. */
}

CtrlMode mode_get(void) {
	return 0;
}

bool new_data_received(void) {
	return false;
}

/*
 * ANALOG AND DIGITAL INPUTS
 */
void pin_set_io(PinIx pin, PinMode mode) {
	/* TODO May not be necessary on the cortex. */
}

uint16_t analog_adc_get(PinIx pin) {
	return GetAnalogInput(pin);
}

int8_t analog_oi_get(PinIx pin) {
	/* TODO Is this the same as analog_adc_get()? */
	return 0;
}

void digital_set(PinIx pin, bool value) {
	SetDigitalOutput(pin, value);
}

int8_t digital_get(PinIx pin) {
	return GetDigitalInput(pin);
}

/*
 * MOTOR AND SERVO OUTPUTS
 */
void analog_set(AnalogOutIx index, AnalogOut out) {
	/* TODO Implement this, perhaps using SetMotor()? */
}

void motor_set(AnalogOutIx index, MotorSpeed out) {
	SetMotor(index, out);
}

void servo_set(AnalogOutIx index, ServoPosition out) {
	SetServo(index, out);
}

/*
 * TIMERS
 */
/* TODO Currently unsupported on both architectures. */

/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */
/* TODO Not possible using the current EasyC code. */


