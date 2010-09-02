/*
 * DIGITAL IO
 */
void digital_init(index_t index, bool output) {
    GPIO_InitTypeDef GPIO_param;
}

bool digital_get(index_t index) {
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (index) {
        /* VEXNet Joystick */
	case PIN_OI(5): /* Left trigger */
		return (joystick->g5_u << 0)
		     | (joystick->g5_d << 1);

	case PIN_OI(6): /* Right trigger */
		return (joystick->g6_u << 0)
		     | (joystick->g6_d << 1);

	case PIN_OI(7): /* Left Buttons */
		return (joystick->g7_u << 0)
		     | (joystick->g7_d << 1)
		     | (joystick->g7_l << 2)
		     | (joystick->g7_r << 3);

	case PIN_OI(8): /* Right Buttons */
		return (joystick->g8_u << 0)
		     | (joystick->g8_d << 1)
		     | (joystick->g8_l << 2)
		     | (joystick->g8_r << 3);

        /* Exposed Pins */
	case PIN_DIGITAL(1):
        case PIN_DIGITAL(2):
        case PIN_DIGITAL(3):
        case PIN_DIGITAL(4):
        case PIN_DIGITAL(5):
        case PIN_DIGITAL(6):
        case PIN_DIGITAL(7):
        case PIN_DIGITAL(8):
        case PIN_DIGITAL(9):
        case PIN_DIGITAL(10):
        case PIN_DIGITAL(11):
        case PIN_DIGITAL(12): {
            GPIO_TypeDef *port = ifipin_to_port[index - OFFSET_DIGITAL - 1];
            index_t       pin  = ifipin_to_pin[index - OFFSET_DIGITAL - 1];
            return !!(port->IDR & (1 << pin));
        }

	default:
		ERROR();
		return 0;
	}
}
