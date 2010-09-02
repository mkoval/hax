/*
 * ANALOG IO
 */
void analog_set(index_t index, int8_t value) {
        uint8_t value2;

        /* Convert the motor speed to an unsigned value. */
        value  = (value < 0 && value != -128) ? value - 1 : value;
        value2 = value + 128;

        switch (index) {
        /* Three-wire Motor/Servo*/
        case PIN_MOTOR(2):
        case PIN_MOTOR(3):
        case PIN_MOTOR(4):
        case PIN_MOTOR(5):
        case PIN_MOTOR(6):
        case PIN_MOTOR(7):
        case PIN_MOTOR(8):
        case PIN_MOTOR(9):
            u2m.u2m.motors[index] = val;
            break;

        /* Two-wire Motor */
        case PIN_MOTOR(1):
        case PIN_MOTOR(10):
            ERROR();
            break;

        default:
            ERROR();
        }
}


int16_t analog_get(index_t id) {
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (id) {
        /* VEXNet Joystick */
        case PIN_OI(1): /* Right Stick, X */
		return joystick->axis_2;

	case PIN_OI(2): /* Right Stick, Y */
		return joystick->axis_1;

	case PIN_OI(4): /* Left Stick, X */
		return joystick->axis_4;

	case PIN_OI(3): /* Left Stick, Y */
		return joystick->axis_3;

        /* ADCs */
	case PIN_DIGITAL(1):
        case PIN_DIGITAL(2):
        case PIN_DIGITAL(3):
        case PIN_DIGITAL(4):
        case PIN_DIGITAL(5):
        case PIN_DIGITAL(6):
        case PIN_DIGITAL(7):
        case PIN_DIGITAL(8):
	 	return adc_buffer[index - 1] >> 2;

    	default:
		ERROR();
		return 0;
	}
}
