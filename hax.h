#ifndef HAX_H_
#define HAX_H_

#include <compilers.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * The Main function
 */
void main(void) __noreturn;

/* Zero-indexed indices for inputs, outputs, and hardware interrupts. */
typedef uint8_t index_t;

/* Callback function invoked when a hardware interrupt is fired. */
typedef void (*isr_t)(int8_t);

/* Operating mode of the robot (autonomous or operator controlled). */
typedef enum {
	MODE_AUTON,
	MODE_TELOP,
	MODE_DISABLE
} mode_t;

#define ANALOG_OUT_MIN (-127)
#define ANALOG_OUT_MAX  (127)

/* Number of microseconds between two consecutive slow loops. Defined by the
 * architecture-specific implementation of HAX.
 */
extern uint16_t const kSlowSpeed;

/* Number of analog inputs, numbered 0 to kNumAnalogInputs - 1. Defined by user
 * code.
 */
extern uint8_t const kNumAnalogInputs;

/*
 * USER CONTROLLER CODE
 */
/* These functions implement a hardware-agnostic robot controller. See loop()
 * and spin() for the difference between the two classes of function.
 */
void init(void);
void auton_loop(void);
void auton_spin(void);
void telop_loop(void);
void telop_spin(void);
void disable_loop(void);
void disable_spin(void);

/*
 * INITIALIZATION AND MISC
 */
/* Hardware-specific initialization code that is executed exactly once
 * prior to any invocations of spin or loop. Responsible for initializing
 * analog inputs, digital inputs, and outputs.
 * setup_1 executes prior to user init, and setup_2 follows user init.
 */
void setup_1(void);
void setup_2(void);

/* Executed whenever the processor is not otherwise occupied (aka. the "fast
 * loop"). Time between two consecutive invocations of this function is
 * non-deterministic and can not be relied upon.
 */
void spin(void);

/* Executed whenever the processor has received new data, every kSlowSpeed
 * milliseconds (aka. the "slow loop"). Controller code should be placed in
 * this function unless there is a compelling reason to place it in spin().
 * loop_1 runs prior to user code, and loop_2 follows it.
 */
void loop_1(void);
void loop_2(void);

/* Check if there is new data available, triggering the invocation of the
 * loop() function.
 */
bool new_data_received(void);

/* Determines if the robot is in autonomous or telop mode. */
CtrlMode mode_get(void);

/* Puts the robot in autonomous or telop mode. */
void mode_set(CtrlMode new_mode);


/*
 * ANALOG AND DIGITAL INPUTS
 */
/* Expected to be invoked exactly once, in the setup() function. */
void pin_set_io(PinIx, PinMode);

/* Get a raw analog value from the input with the specified Ix. Produces
 * undefined results if the input is configured as a digital
 * sensor.
 */
int8_t analog_oi_get(OIIx);
uint16_t analog_adc_get(PinIx);

/* Gets and sets digital values for the specified port number. Produces
 * undefined results if the input is configured as an analog sensor.
 */
void digital_set(PinIx, bool);
bool digital_get(PinIx);

/*
 * MOTOR AND SERVO OUTPUTS
 */
/* More raw function, bounded by kAnalogOut{Max,Min} */
void analog_set(AnalogOutIx, AnalogOut);

/* Motor's speed must be bounded by kMotorMin and kMotorMax. */
void motor_set(AnalogOutIx, MotorSpeed);

/* Servo's position must be bounded by kServoMin and kServoMax. */
void servo_set(AnalogOutIx, ServoPosition);

/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */
/* Sets the ISR callback function to be invoked when this interrupt occurs. */
void interrupt_reg_isr(InterruptIx, InterruptServiceRoutine);

/* Reads from an interrupt port as if it is a digital input. */
bool interrupt_get(InterruptIx);

/* Enable and disable interrupts to prevent an ISR from being invoked in
 * potentially dangerous locations and in initialization.
 */
void interrupt_enable(InterruptIx);
void interrupt_disable(InterruptIx);

/*
 * TIMERS
 */
/* Enable or disable the specified timer. */
void timer_set(TimerIx, bool);

/* Reads the curent value of the specified timer. */
uint16_t timer_read(TimerIx);

/* Loads the a new value into the specified timer. */
void timer_write(TimerIx, uint16_t);

/*
 * STREAM IO
 */
/* Writes data of the specified type to stdout, formatting it in a human-
 * readable manner. Hex values, written with puth, are not prefixed with "0x"
 * unless manually printed with the other put() methods.
 */
void _putc(char);
void puth(uint16_t);
void _puth(uint8_t);
void _puth2(uint16_t);
void _puti(uint16_t);
void _putf(float);
void _puts(char const __rom *c);


#endif
