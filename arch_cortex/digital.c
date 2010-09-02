/*
 * DIGITAL IO
 */
void digital_init(index_t index, bool output)
{
	// Only external digital pins can be used as output.
	if (index < OFFSET_DIGITAL || index >= OFFSET_DIGITAL + CT_DIGITAL) {
		ERROR();
	} else {
		GPIO_InitTypeDef GPIO_param;
		GPIO_param.GPIO_Pin = 1 << ifipin_to_pin[index - 1];
		if (output) {
			GPIO_param.GPIO_Mode  = GPIO_Mode_IPU;
			GPIO_param.GPIO_Speed = GPIO_Speed_500MHz;
		} else {
			GPIO_param.GPIO_Mode  = GPIO_Mode_Out_PP;
		}
	}
}

bool digital_get(index_t index)
{
	struct oi_data *oi1 = &m2u.m2u.joysticks[0].b;
	struct oi_data *oi2 = &m2u.m2u.joysticks[1].b;

	switch (index) {
	/* Left Trigger */
	case PIN_OI_BUTTON(1, 5, OI_B_UP):
		return oi1->g5_u;
	case PIN_OI_BUTTON(1, 5, OI_B_DN):
		return oi1->g5_d;
	case PIN_OI_BUTTON(2, 5, OI_B_UP):
		return oi2->g5_u;
	case PIN_OI_BUTTON(2, 5, OI_B_DN):
		return oi2->g5_d;

	/* Right Trigger */
	case PIN_OI_BUTTON(1, 6, OI_B_UP):
		return oi1->g6_u;
	case PIN_OI_BUTTON(1, 6, OI_B_DN):
		return oi1->g6_d;
	case PIN_OI_BUTTON(2, 6, OI_B_UP):
		return oi2->g6_u;
	case PIN_OI_BUTTON(2, 6, OI_B_DN):
		return oi2->g6_d;

	/* Left D-Pad */
	case PIN_OI_BUTTON(1, 7, OI_B_UP):
		return oi1->g7_u;
	case PIN_OI_BUTTON(1, 7, OI_B_DN):
		return oi1->g7_d;
	case PIN_OI_BUTTON(1, 7, OI_B_LT):
		return oi1->g7_l;
	case PIN_OI_BUTTON(1, 7, OI_B_RT):
		return oi1->g7_r;
	case PIN_OI_BUTTON(2, 7, OI_B_UP):
		return oi2->g7_u;
	case PIN_OI_BUTTON(2, 7, OI_B_DN):
		return oi2->g7_d;
	case PIN_OI_BUTTON(2, 7, OI_B_LT):
		return oi2->g7_l;
	case PIN_OI_BUTTON(2, 7, OI_B_RT):
		return oi2->g7_r;

	/* Exposed Digital Pins */
	case OFFSET_DIGITAL ... (OFFSET_DIGITAL + CT_DIGITAL - 1):
		GPIO_TypeDef * port = ifipin_to_port[index - OFFSET_DIGITAL - 1];
		index_t         pin = ifipin_to_pin[index - OFFSET_DIGITAL - 1];
		return !!(port->IDR & (1 << pin));

	default:
		ERROR();
		return 0;
	}
}

void digital_set(index_t index, bool output)
{

}
