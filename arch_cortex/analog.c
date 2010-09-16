#include <stdint.h>
#include <hax.h>
#include "spi.h"

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
		WARN("index %d; value %d", index, value);
	}
}

uint16_t analog_get(index_t ix)
{
	if (IX_ANALOG(1) <= ix && ix <= IX_ANALOG(CT_ANALOG)) {
		return adc_buffer[IX_ANALOG_INV(ix)];
	} else {
		WARN_IX(ix);
		return 0;
	}
}
