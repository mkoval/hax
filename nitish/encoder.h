#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>

void encoder_0a(int8_t);
void encoder_0b(int8_t);
void encoder_1a(int8_t);
void encoder_1b(int8_t);
void encoder_2a(int8_t);
void encoder_2b(int8_t);

void encoder_reset(void);

/* Gets the total number of encoder ticks registered by the specified encoder. */
int32_t encoder_get(uint8_t n);

#endif
