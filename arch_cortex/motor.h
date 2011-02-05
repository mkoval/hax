
#ifndef HAX_MOTOR_H_
#define HAX_MOTOR_H_

#include <stdint.h>

void motors_init(void);
void motor0_set(uint16_t pwr);
void motor1_set(uint16_t pwr);

#endif
