#ifndef RCC_H_


uint64_t time_get_ms(void);
void udelay_500(void);

// Initialize the cortex's clocks.
void clocks_init(void);

#endif
