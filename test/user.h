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

void init(void);
void auton_loop(void);
void telop_loop(void);
void disable_loop(void);
void auton_spin(void);
void telop_spin(void);
void disable_spin(void);
#endif
