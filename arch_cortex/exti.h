#ifndef EXTI_H_
#define EXTI_H_

void exti_init(void);

extern isr_t isr_callback[12];
extern GPIO_TypeDef *const ifipin_to_port[12];
extern const int8_t ifipin_to_pin[12];
#endif
