/*
 * ANALOG IO
 */
void analog_set(index_t index, int8_t value) {
	uint8_t value2;

	/* Convert the motor speed to an unsigned value. */
	value  = (value < 0 && value != -128) ? value - 1 : value;
	value2 = value + 128;

	if (OFFSET_ANALOG <= index && index <= OFFSET_ANALOG + CT_ANALOG) {
		u2m.u2m.motors[index] = val;
	} else {
		ERROR();
	}
}


int16_t analog_get(index_t id) {
	struct oi_data *oi1 = &m2u.m2u.joysticks[0].b;
	struct oi_data *oi2 = &m2u.m2u.joysticks[1].b;

	switch (id) {
	/* Right Joystick */
	case JOY_R_Y(1):
		return oi1->axis_1;
	case JOY_R_X(1):
		return oi1->axis_2;
	case JOY_R_Y(2):
		return oi2->axis_1;
	case JOY_R_X(2):
		return oi2->axis_2;

	/* Left Joystick */
	case JOY_L_Y(1):
		return oi1->axis_3;
	case JOY_L_X(1):
		return oi1->axis_4;
	case JOY_L_Y(2):
		return oi1->axis_3;
	case JOY_L_X(2):
		return oi1->axis_4;

	/* ADCs */
	case OFFSET_DIGITAL ... (OFFSET_DIGITAL + CT_DIGITAL - 1):
		return adc_buffer[index - 1] >> 2;

	default:
		ERROR();
		return 0;
	}
}
