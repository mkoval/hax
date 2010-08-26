#ifndef ARCH_PIC_H_
#define ARCH_PIC_H_

#define PIN_DIGITAL(_x_)   ((1 <= (_x_) && (_x_) <= 16) ? (_x_) + 0  : 0)
#define PIN_INTERRUPT(_x_) ((1 <= (_x_) && (_x_) <= 6)  ? (_x_) + 16 : 0)
#define PIN_MOTOR(_x_)     ((1 <= (_x_) && (_x_) <= 8)  ? (_x_) + 22 : 0)
#define PIN_OI(_x_)        ((1 <= (_x_) && (_x_) <= 6)  ? (_x_) + 30 : 0)

#endif
