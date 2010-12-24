#ifndef USER_H_
#define USER_H_

#include <stdbool.h>
#include <stdint.h>

extern volatile int32_t enc_left;
extern volatile int32_t enc_right;

void enc_left_a(bool);
void enc_left_b(bool);
void enc_right_a(bool);
void enc_right_b(bool);

#endif
