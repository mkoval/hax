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
		ERROR("index %d; value %d", index, value);
	}
}

int8_t oi_group_get(index_t ix)
{
	uint8_t gr = IX_OI_GROUP_INV(ix);
	struct oi_date *oi = &m2u.m2u.joystics[IX_OI_OI_INV(ix)].b;
	uint8_t data;
	switch(gr) {
	case 1:
		data = oi->axis_1;
		break;
	case 2:
		data = oi->axis_2;
		break;
	case 3:
		data = oi->axis_3;
		break;
	case 4:
		data = oi->axis_4;
		break;
	/* Triggers (groups of 2) */
	case 5:
		return 127*(oi->g5_u) - 127*(oi->g5_d)
	case 6:
		return 127*(oi->g6_u) - 127*(oi->g6_d)
	/* Buttons (groups of 4) */
	case 7:
	case 8:
		ERROR("idx %d", ix);
		return 0;
	/* accelerometer data */
	case 9:
		data = oi->accel_x;
		break;
	case 10:
		data = oi->accel_y;
		break;
	case 11:
		data = oi->accel_z;
		break;
	default:
		ERROR("idx %d", ix);
		return 0;
	}

	return (data == -128)?(-127):(data);
}

int8_t oi_rocker_get(index_t ix)
{
	struct oi_data *oi [] = { &m2u.m2u.joystick[0].b, &m2u.m2u.joystick[1].b };
	switch(ix) {
	case IX_OI(1, 5):
		return oi[0]->g5_u - oi[0]->g5_d;
	case IX_OI(1, 6):
		return oi[0]->g6_u - oi[0]->g6_d;
	case IX_OI(2, 5):
		return oi[1]->g5_u - oi[1]->g5_d;
	case IX_OI(2, 6):
		return oi[1]->g6_u - oi[1]->g6_d;
	default:
		ERROR("idx: %d", ix);
		return 0;
	}
}

bool oi_button_get(index_t ix)
{
	uint8_t oi_i = IX_OI_BUTTON_OI_INV(ix);
	struct oi_data *oi = &m2u.m2u.joystick[oi_i].b;
	index_t i = IX_OI_BUTTONx_INV(ix, oi_i);

	switch(i) {
	case IX_OI_BUTTON(1, 5, OI_B_UP):
		return oi->g5_u;
	case IX_OI_BUTTON(1, 5, OI_B_DN):
		return oi->g5_d;
	case IX_OI_BUTTON(1, 6, OI_B_UP):
		return oi->g6_u;
	case IX_OI_BUTTON(1, 6, OI_B_DN):
		return oi->g6_d;

	case IX_OI_BUTTON(1, 7, OI_B_UP):
		return oi->g7_u;
	case IX_OI_BUTTON(1, 7, OI_B_LT):
		return oi->g7_l;
	case IX_OI_BUTTON(1, 7, OI_B_DN):
		return oi->g7_d;
	case IX_OI_BUTTON(1, 7, OI_B_RT):
		return oi->g7_r;

	case IX_OI_BUTTON(1, 8, OI_B_UP):
		return oi->g8_u;
	case IX_OI_BUTTON(1, 8, OI_B_LT):
		return oi->g8_l;
	case IX_OI_BUTTON(1, 8, OI_B_DN):
		return oi->g8_d;
	case IX_OI_BUTTON(1, 8, OI_B_RT):
		return oi->g8_r;

	default:
		ERROR("idx: %d", ix);
	}
}

uint16_t analog_get(index_t ix)
{
	if (IX_ANALOG(1) <= ix && ix <= IX_ANALOG(CT_ANALOG)) {
		return adc_buffer[IX_ANALOG_INV(ix)];
	} else {
		ERROR("idx: %d", ix);
		return 0;
	}
}
