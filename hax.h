/*
	API For accessing embedded systems functionality
*/

// XXX: Can this be relied on? (My sense is no, thus we'll need some "haxint.h")
#inlcude <stdint.h>

typedef uint8_t Ix; // Indexing type



//  We either do this (which leads to the boiler plate code in "NOTDEF", or
//   have the user supply functions 'init','auton','telop', etc.
// Some semi-vex specific things
void hw_setup(void); // hardware specific initialization code.
void hw_spin(void);  // execute whenever we have time.
void hw_slow_spin(void); // execute whenever the outputs update.

extern const unsigned int kSlowSpeed; // how fast is the slow loop?

BOOL new_data_recved(void); // if we have new datas.
RobotMode get_mode(void); // the mode (Auton/Telop)

#ifdef NOTDEF
void main(void) __noreturn;
void main() {
	RobotMode mode;
	hw_setup();
	init();
	for(;;) {
		mode = get_mode();
		if (new_data_recved()) {
			hw_slow_spin();
			user_slow();
			if (mode==kAuton) {
				auton_slow();
			}
			else {
				telop_slow();
			}
		}
		hw_spin();
		user_fast();
		if (mode==kAuton) {
			auton_fast();
		}
		else {
			telop_fast();
		}
	}
}
#endif



// Analog inputs 
//  includes on-robot sensors and remote inputs
Analog analog_get(Ix input_num);

// Digital I/O
void digital_set(Ix output_num);
BOOL digital_get(Ix input_num);

// Interupts
//  Could also give a bit less abstraction than this. Something like calling a
//  specific funtion for each interupt, and looking for them to enable said
//  interupt
//
//  Also, some interupts may need more config than on/off.
typedef void(*)(void) InterruptServiceRoutine;
reg_isr(Ix interrupt_num, InterruptServiceRoutine routine);
enable_interupt(Ix inter_num);
disable_interupt(Ix inter_num);


// Motors
//  We should define some type of range for all motors, and then convert for
//  motors than need different values.
typedef MotorSpeed int16_t;
enum {
	kMotorMax=127,
	kMotorMin=-127
};
void motor_set(Ix motor_num, MotorSpeed new_speed);	

// Servos
//  Same thing as motors on pic. May not be elsewhere.
typedef ServoPosition int16_t;
enum {
	kServoMax=127,
	kServoMin=-127
};
void servo_set(Ix servo_num, ServoPosition new_setpoint);

// Stream I/O (something for stdin/stdout)
//  Shall we use standard io or nix it? (I'm all for avoiding format strings
//  chewing up memory)
void puth(uint16_t hex);
void puts(const char *c);
//getc?

// Configuration
//  inputs need to be set to analog or digital.
//  outputs may or may not need something similar
typedef enum {
	kInput,
	kOutput
} PinMode;
void intput_mode(Ix pin, PinMode mode );
