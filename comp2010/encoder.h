#ifndef ENCODER_COMP2010_H_
#define ENCODER_COMP2010_H_

#include <hax.h>
#include <stdint.h>

/* Arbitrary indexes assigned to the encoders to simplify this API. */
typedef uint8_t EncoderIx;

/* Associate two interrupt pins with a logical encoder index. Also enables the
 * references interrutps if they were not already.
 */
void encoder_init(EncoderIx, InterruptIx, InterruptIx);

/* Get the number of encoder ticks registered by the specified encoder since
 * the last call to encoder_reset().
 */
int32_t encoder_get(EncoderIx);

/* Reset the number of encoder ticks recorded by an encoder to zero. */
void encoder_reset(EncoderIx);

/* Equivalent to calling encoder_reset() on each encoder. */
void encoder_reset_all(void);

/* Interrupt service routine callbacks for internal use. */
void encoder_0a(int8_t);
void encoder_0b(int8_t);
void encoder_1a(int8_t);
void encoder_1b(int8_t);
void encoder_2a(int8_t);
void encoder_2b(int8_t);

#endif
