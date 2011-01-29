#ifndef RCC_H_

/* Tracks our clock source so we can guess at frequency
 *  HSE => 72 MHz
 *  HSI => 64 MHz
 */
#define CS_HSE 0
#define CS_HSI 1
extern int rcc_src_hclk;

uint64_t time_get_ms(void);

// Initialize the cortex's clocks.
void clocks_init(void);

#endif
