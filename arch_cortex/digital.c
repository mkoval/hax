/*
 * DIGITAL IO
 */
void digital_init(index_t index, bool output) {
    GPIO_InitTypeDef GPIO_param;

    GPIO_param.GPIO_Pin = (uint16_t)(1 << ifipin_to_pin[index - 1]);

    if (output) {
        ifipin_to_port[index - 1]->BSRR = 1 << ifipin_to_pin[index - 1];
    } else {
        ifipin_to_port[index - 1]->BRR  = 1 << ifipin_to_pin[index - 1];
    }
}

bool digital_get(index_t index) {
	struct oi_data *oi1 = &m2u.m2u.joysticks[0].b;
        struct oi_data *oi2 = &m2u.m2u.joysticks[1].b;

        switch (index) {
        case: /* OI1, Left Trigger, Up */
            return oi1->g5_u;
        case: /* OI1, Left Trigger, Down */
            return oi1->g5_d;
        case: /* OI1, Right Trigger, Up */
            return oi1->g6_u;
        case: /* OI1, Right Trigger, Down */
            return oi1->g6_d;
        case: /* OI1, Left D-Pad, Up */
            return oi1->g7_u;
        case: /* OI1, Left D-Pad, Down */
            return oi1->g7_d;
        case: /* OI1, Left D-Pad, Left */
            return oi1->g7_l;
        case: /* OI1, Left D-Pad, Right */
            return oi1->g7_r;
        case: /* OI2, Left Trigger, Up */
            return oi2->g5_u;
        case: /* OI2, Left Trigger, Down */
            return oi2->g5_d;
        case: /* OI2, Right Trigger, Up */
            return oi2->g6_u;
        case: /* OI2, Right Trigger, Down */
            return oi2->g6_d;
        case: /* OI2, Left D-Pad, Up */
            return oi2->g7_u;
        case: /* OI2, Left D-Pad, Down */
            return oi2->g7_d;
        case: /* OI2, Left D-Pad, Left */
            return oi2->g7_l;
        case: /* OI2, Left D-Pad, Right */
            return oi2->g7_r;
        case: /* Exposed Digital Pins */ {
            GPIO_TypeDef *port = ifipin_to_port[index - OFFSET_DIGITAL - 1];
            index_t       pin  = ifipin_to_pin[index - OFFSET_DIGITAL - 1];
            return !!(port->IDR & (1 << pin));
        }
        default:
            ERROR();
            return 0;
        }
#if 0
#endif
}

void digital_set(index_t index, bool output) {
    
}
