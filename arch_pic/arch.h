#ifndef ARCH_PIC_H_
#define ARCH_PIC_H_

#define PIN_DIGITAL(_x_)   ((_x_) + 0)  /*  1 - 16 */
#define PIN_INTERRUPT(_x_) ((_x_) + 16) /* 17 - 23 */
#define PIN_MOTOR(_x_)     ((_x_) + 22) /* 24 - 32 */
#define PIN_OI(_x_)        ((_x_) + 41) /* 33 - 41 */

#endif
