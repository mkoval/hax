
#ifndef HAX_MOTOR_H_
#define HAX_MOTOR_H_

#include <stdint.h>

void motors_init(void);
void motor0_set(int16_t pwr);
void motor1_set(int16_t pwr);

#endif
