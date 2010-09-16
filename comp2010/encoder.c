#include <hax.h>
#include <stdbool.h>
#include <stdint.h>

#include "encoder.h"

static bool intenc[6] = {};

/* Mapping between logical "encoder index" and interrupt pins indexes. */
static volatile index_t encmap[6];

/* Store ticks in each direction in different memory locations to avoid
 * corruption from small oscillations.
 */
static volatile uint32_t counts[6];

#define ENC(flip, other_map_idx, pin_map_idx) \
	ENCODER(flip, encmap[other_map_idx], encmap[pin_map_idx])
#define ENCODER(flip, other_pin, pin_idx) \
	ENCODER_(flip, other_pin, (pin_idx) - IX_INTERRUPT(1))
#define ENCODER_(_flip_, _other_, _num_)     \
    do {                                     \
        bool other = digital_get(_other_);   \
        if (_flip_) {                        \
            if (other)                       \
                ++counts[2 *(_num_) + 1];    \
            else                             \
                ++counts[2 *(_num_)];        \
        } else {                             \
            if (other)                       \
                ++counts[2 *(_num_)];        \
            else                             \
                ++counts[2 *(_num_) + 1];    \
        }                                    \
    } while (0)

static void encoder_0a(bool l) {
	ENC( l, 1, 0);
}

static void encoder_0b(bool l) {
	ENC(!l, 0, 1);
}

static void encoder_1a(bool l) {
	ENC( l, 3, 2);
}

static void encoder_1b(bool l) {
	ENC(!l, 2, 3);
}

static void encoder_2a(bool l) {
	ENC( l, 5, 4);
}

static void encoder_2b(bool l) {
	ENC(!l, 4, 5);
}

void encoder_init(uint8_t id, index_t int1, index_t int2) {
	encmap[2 * id + 0] = int1;
	encmap[2 * id + 1] = int2;

	/* Note that interrupts are enabled. */
	intenc[2 * id + 0] = true;
	intenc[2 * id + 1] = true;

	switch (id) {
	case 0:
		interrupt_setup(int1, encoder_0a);
		interrupt_setup(int2, encoder_0b);
		break;

	case 1:
		interrupt_setup(int1, encoder_1a);
		interrupt_setup(int2, encoder_1b);
		break;

	case 2:
		interrupt_setup(int1, encoder_2a);
		interrupt_setup(int2, encoder_2b);
		break;
	}

	/* Enable interrupts for both encoder connections. */
	interrupt_set(int1, true);
	interrupt_set(int2, true);
}

void encoder_update(void) {
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

