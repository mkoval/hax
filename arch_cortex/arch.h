#ifndef ARCH_CORTEX_H_
#define ARCH_CORTEX_H_

/* Period of the slow loop, in microseconds. */
#define SLOW_US 18200

/* Offset for each block of contiguous pins. */
#define OFFSET_ANALOG     0
#define OFFSET_DIGITAL    8
#define OFFSET_MOTOR     20
#define OFFSET_OI        32
#define OFFSET_OI_BUTTON 40

#define OI_B_UP 0
#define OI_B_LT 1
#define OI_B_DN 2
#define OI_B_RT 3

/* Utility macros for generating internal pin indexes. */
#define PIN_ANALOG(_x_)  ((_x_) + OFFSET_ANALOG)
#define PIN_DIGITAL(_x_) ((_x_) + OFFSET_DIGITAL)
#define PIN_MOTOR(_x_)   ((_x_) + OFFSET_MOTOR)
#define PIN_OI(_x_)      ((_x_) + OFFSET_OI)
#define PIN_OI_BUTTON(n, dir) (OFFSET_OI_BUTTON + ((n)*4) + (dir))

/* Check a single button press on one of the VEXNet joystick's button pads. */
#define BUT_UP(_x_)      ((_x_) & 1 != 0)
#define BUT_DOWN(_x_)    ((_x_) & 2 != 0)
#define BUT_LEFT(_x_)    ((_x_) & 4 != 0)
#define BUT_RIGHT(_x_)   ((_x_) & 8 != 0)

#endif
