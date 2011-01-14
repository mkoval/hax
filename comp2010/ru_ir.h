#ifndef _ru_ir_h
#define _ru_ir_h

#include <stdint.h>

#define Get_Front_IR()      (ir_long_to_in10(ir_filters[0].output))
#define Get_Side_Front_IR() (ir_long_to_in10(ir_filters[1].output))
#define Get_Side_Rear_IR()  (ir_long_to_in10(ir_filters[2].output))
#define Get_Rear_IR()       (ir_short_to_in10(ir_filters[3].output))
#define Get_Front_Low_IR()  (ir_short_to_in10(ir_filters[4].output))

#define Get_Front_IR_In()      (Get_Front_IR()/10.0)
#define Get_Side_Front_IR_In() (Get_Side_Front_IR()/10.0)
#define Get_Side_Rear_IR_In()  (Get_Side_Rear_IR()/10.0)
#define Get_Rear_IR_In()       (Get_Rear_IR()/10.0)
#define Get_Front_Low_IR_In()  (Get_Front_Low_IR()/10.0)

int ir_long_to_in10(int sig);
int ir_short_to_in10(int sig);

// ------------------------------------------------------------

#define num_ir_sensors 5

typedef struct {
	uint8_t ana_port;
	int output;
	int s1;
	int s2;
} ir_filter_t;

extern ir_filter_t ir_filters[num_ir_sensors];

void ir_filter_routine(void);

#endif
