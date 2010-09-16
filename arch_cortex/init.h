#ifndef CORTEX_INIT_H_
#define CORTEX_INIT_H_ 1

#include <stdint.h>

#define ADC_NUM 8

extern volatile uint16_t adc_buffer[ADC_NUM];

void gpio_init(void);
void adc_init(void);
void nvic_init(void);
void tim1_init(void);

#endif
