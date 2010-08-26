#ifndef ARCH_CORTEX_H_
#define ARCH_CORTEX_H_

#define PIN_ANALOG(_x_)  ((1 <= (_x_) && (_x_) <= 8)  ? (_x_) + 0  : 0)
#define PIN_DIGITAL(_x_) ((1 <= (_x_) && (_x_) <= 12) ? (_x_) + 8  : 0)
#define PIN_MOTOR(_x_)   ((1 <= (_x_) && (_x_) <= 10) ? (_x_) + 20 : 0)

#endif
