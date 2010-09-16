#ifndef ENCODER_COMP2010_H_
#define ENCODER_COMP2010_H_

#include <hax.h>
#include <stdint.h>


enum {
	ENC_L,
	ENC_R
};

/* Arbitrary indexes assigned to the encoders to simplify this API. */
typedef uint8_t EncoderIx;

/* Associate two interrupt pins with a logical encoder index. Also enables the
 * references interrutps if they were not already.
 */
void encoder_init(EncoderIx, index_t int1, index_t int2);

/* Get the number of encoder ticks registered by the specified encoder since
 * the last call to encoder_reset().
 */
int32_t encoder_get(EncoderIx);

/* Simulate interrupts with digital ports if necessary. */
void encoder_update(void);

/* Reset the number of encoder ticks recorded by an encoder to zero. */
void encoder_reset(EncoderIx);

/* Equivalent to calling encoder_reset() on each encoder. */
void encoder_reset_all(void);

#endif
