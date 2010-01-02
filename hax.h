#ifndef HAX_H_
#define HAX_H_

#include <stdint.h>

/* To avoid potentially different definitions of true (e.g. 0x01 versus 0xFF),
 * never compare against kFalse or kTrue. Instead rely on the default behavior
 * of the if-statement that considers a non-zero value to be true.the
 */
typedef int   BOOL;
#define FALSE 0
#define TRUE  !FALSE

/* Zero-indexed indices for inputs, outputs, and hardware interrupts. */
typedef uint8_t InputIndex;
typedef uint8_t OutputIndex;
typedef uint8_t InterruptIndex;

/* Configuration options to be applied to each input. */
typedef enum {
	kInput,
	kOutput
} PortType;

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
typedef void(*)(void) InteruptServiceRoutine;

/* Operating mode of the robot (autonomous or operator controlled). */
typedef enum {
	kAuton,
	kTelop
} Mode;

/* Number of milliseconds between two consecutive instances of the processor
 * receiving updated data (i.e. the speed of the "slow loop").
 */
extern uint16_t kSlowSpeed;

/* Number of analog inputs, numbered 0 to kNumAnalog - 1. This numbering
 * scheme is required for portability on PIC hardware.
 */
extern uint8_t kNumAnalog;


/*
 * USER CONTROLLER CODE
 */
/* These functions implement a hardware-agnostic robot controller. See loop()
 * and spin() for the difference between the two classes of function.
 */
void auton_loop();
void auton_spin();
void telop_loop();
void telop_spin();


/*
 * INITIALIZATION AND MISC
 */
/* Hardware-specific initialization code that is executed exactly once
 * prior to any invocations of spin or loop. Responsible for initializing
 * analog inputs, digital inputs, and outputs.
 */
void setup(void);

/* Executed whenever the processor is not otherwise occupied (aka. the "fast
 * loop"). Time between two consecutive invocations of this function is
 * non-deterministic and can not be relied upon.
 */
void spin(void);

/* Executed whenever the processor has received new data, every kSlowSpeed
 * milliseconds (aka. the "slow loop"). Controller code should be placed in
 * this function unless there is a compelling reason to place it in spin().
 */
void loop(void);

/* Check if there is new data available, triggering the invocation of the
 * loop() function.
 */
Bool new_data_received(void);

/* Determines if the robot is in autonomous or operator-control mode */
Mode get_mode(void); // the mode (Auton/Telop)


/*
 * ANALOG AND DIGITAL INPUTS
 */
/* Expected to be invoked exactly once, in the setup() function. */
void init_port(PortType);

/* Get a raw analog value from the input with the specified index. Produces
 * undefined results if the input is configured as a digital
 * sensor.
 */
Analog analog_get(InputIndex);

/* Gets and sets digital values for the specified port number. Produces
 * undefined results if the input is configured as an analog sensor.
 */
void digital_set(Index);
Bool digital_get(Index);


/*
 * MOTOR AND SERVO OUTPUTS
 */
/* Motor's speed must be bounded by kMotorMin and kMotorMax. */
void motor_set(OutputIndex, MotorSpeed);

/* Servo's position must be bounded by kServoMin and kServoMax. */
void servo_set(Ix servo_num, ServoPosition new_setpoint);


/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */
/* Sets the ISR callback function to be invoked when this interrupt occurs. */
set_isr(InteruptIndex, InterruptServiceRoutine);

/* Enable and disable interrupts to prevent an ISR from being invoked in
 * potentially dangerous locations.
 */
set_interrupt_enabled(InterruptIndex, Bool);
enable_interrupt(InterruptIndex);
disable_interrupt(InterruptIndex);


/*
 * STREAM IO
 */
/* Writes data of the specified type to stdout, formatting it in a human-
 * readable manner. Hex values, written with puth,  are prefixed with "0x".
 */
void puth(uint16_t);
void puti(uint16_t);
void putf(float);
void puts(char *c);

#endif
