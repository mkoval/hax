#ifndef HAX_H_
#define HAX_H_

#include "stdint.h"
#include "compilers.h"

/*
 * The Main function
 */
void main(void) __noreturn;

/* To avoid potentially different definitions of true (e.g. 0x01 versus 0xFF),
 * never compare against kFalse or kTrue. Instead, rely on the default
 * behavior of if statements that consider non-zero values to be true.
 */
typedef uint8_t Bool;
#define FALSE   0
#define TRUE    1

/* Zero-indexed indices for inputs, outputs, and hardware interrupts. */
typedef uint8_t PinIndex; /* Pins on the RC */
typedef uint8_t AnalogInIndex;  /* The union of OI inputs and the PinIndex*/
typedef uint8_t AnalogOutIndex; /* Just the PWMs */
typedef uint8_t InterruptIndex;

/* Configuration options to be applied to each input. */
typedef enum {
	kInput,
	kOutput
} PinMode;

/* Motor speed type, where kMotorMin represents full reverse speed
 * and kMotorMax represents full foward speed.
 */
typedef int8_t MotorSpeed;
#define kMotorMin -127
#define kMotorMax +127

/* Severo position type, where kServoMin and kServoMax are the two extremes
 * of the servo's range of motion.
 */
typedef int8_t ServoPosition;
#define kServoMin -127
#define kServoMax +127

/* Callback function invoked when a hardware interrupt is fired. */
typedef void (*InterruptServiceRoutine)(void);

/* Operating mode of the robot (autonomous or operator controlled). */
typedef enum {
	kAuton,
	kTelop
} CtrlMode;

/* Number of microseconds between two consecutive instances of the processor
 * receiving updated data (i.e. the speed of the "slow loop").
 * NOTE: Supplied by the hardware-specific implementation of HAX.
 */
extern uint16_t kSlowSpeed;

/* Number of analog inputs, numbered 0 to kNumAnalogInputs - 1. This 
 * numbering scheme is required for portability on PIC hardware.
 * NOTE: Supplied by user code.
 */
extern uint8_t kNumAnalogInputs;

/* The number below which Analog inputs are ADC, and above which are OI */
#define kAnalogSplit 127

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
Bool new_data_received(void);

/* Determines if the robot is in autonomous or operator-control mode */
CtrlMode get_mode(void);


/*
 * ANALOG AND DIGITAL INPUTS
 */
/* Expected to be invoked exactly once, in the setup() function. */
void pin_set_io(PinIndex, PinMode);

/* Get a raw analog value from the input with the specified index. Produces
 * undefined results if the input is configured as a digital
 * sensor.
 */
uint16_t analog_get(AnalogInIndex);

/* Gets and sets digital values for the specified port number. Produces
 * undefined results if the input is configured as an analog sensor.
 */
void digital_set(PinIndex, Bool);
Bool digital_get(PinIndex);


/*
 * MOTOR AND SERVO OUTPUTS
 */
/* Motor's speed must be bounded by kMotorMin and kMotorMax. */
void motor_set(AnalogOutIndex, MotorSpeed);

/* Servo's position must be bounded by kServoMin and kServoMax. */
void servo_set(AnalogOutIndex, ServoPosition);


/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */
/* Sets the ISR callback function to be invoked when this interrupt occurs. */
void interrupt_reg_isr(InterruptIndex, InterruptServiceRoutine);

/* Enable and disable interrupts to prevent an ISR from being invoked in
 * potentially dangerous locations. 
 * And so that you can actually enable them in the first place.
 */
void interrupt_set(InterruptIndex, Bool);
void interrupt_enable(InterruptIndex);
void interrupt_disable(InterruptIndex);


/*
 * STREAM IO
 */
/* Writes data of the specified type to stdout, formatting it in a human-
 * readable manner. Hex values, written with puth, are not prefixed with "0x"
 * unless manually printed with the other put() methods.
 */
void putc(char);
void puth(uint16_t);
void puti(uint16_t);
void putf(float);
void puts(char *c);


#endif
