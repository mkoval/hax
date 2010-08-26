#ifndef ARCH_CORTEX_H_
#define ARCH_CORTEX_H_

/* Period of the slow loop, in microseconds. */
#define SLOW_US 18200

/* Utility macros for generating internal pin indexes. */
#define PIN_ANALOG(_x_)  ((_x_) - 0)  /*  1 -  8 */
#define PIN_DIGITAL(_x_) ((_x_) + 8)  /*  8 - 20 */
#define PIN_MOTOR(_x_)   ((_x_) + 20) /* 21 - 31 */
#define PIN_OI(_x_)      ((_x_) + 32) /* 32 - 40 */

#endif
