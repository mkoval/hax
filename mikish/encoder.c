#include <stdbool.h>
#include <stdint.h>

#include "../hax.h"

#include "encoder.h"

/* Mapping between logical "encoder index" and interrupt pins indexes. */
static volatile InterruptIx encmap[6];

/* Store ticks in each direction in different memory locations to avoid
 * corruption from small oscillations.
 */
static volatile uint32_t counts[6];

#define ENCODER(_flip_,_other_,_num_)        \
    do {                                     \
        bool other = interrupt_get(_other_); \
        if (_flip_) {                        \
            if (other)                       \
                ++counts[2 *_num_ + 1];      \
            else                             \
                ++counts[2 * _num_];         \
        } else {                             \
            if (other)                       \
                ++counts[2 * _num_];         \
            else                             \
                ++counts[2 * _num_ + 1];     \
        }                                    \
    } while (0)

void encoder_init(uint8_t id, InterruptIx int1, InterruptIx int2) {
	encmap[2 * id + 0] = int1;
	encmap[2 * id + 1] = int2;

	/* Enable interrupts for both encoder connections. */
	interrupt_enable(int1);
	interrupt_enable(int2);

	switch (id) {
	case 0:
		interrupt_reg_isr(0, encoder_0a);
		interrupt_reg_isr(1, encoder_0b);
		break;
	
	case 1:
		interrupt_reg_isr(2, encoder_1a);
		interrupt_reg_isr(3, encoder_1b);
		break;
	
	case 2:
		interrupt_reg_isr(4, encoder_2a);
		interrupt_reg_isr(5, encoder_2b);
		break;
	}
}

int32_t encoder_get(uint8_t id) {
	return counts[2 * id] - counts[2 * id + 1];
}

void encoder_reset(uint8_t id) {
	counts[id + 0] = 0;
	counts[id + 1] = 0;
}

void encoder_reset_all(void) {
	uint8_t i;
	for (i = 0; i < 6; ++i) {
		counts[i] = 0;
	}
}

void encoder_0a(int8_t l) {
	ENCODER(l, encmap[1], 0);
}

void encoder_0b(int8_t l) {
	ENCODER(!l, encmap[0], 0);
}

void encoder_1a(int8_t l) {
	ENCODER(l, encmap[3], 1);
}

void encoder_1b(int8_t l) {
	ENCODER(!l, encmap[2], 1);
}

void encoder_2a(int8_t l) {
	ENCODER(l, encmap[5], 2);
}

void encoder_2b(int8_t l) {
	ENCODER(!l, encmap[4], 2);
}
