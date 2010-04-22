#include "ru_ir.h"

#include "ports.h"

#define IR_LONG_TABLE_SIZE  256
#define IR_SHORT_TABLE_SIZE 256

#if defined(ROBOT_KEVIN)
ir_filter_t ir_filters[num_ir_sensors] = {
	{ 0 },
	{ 0 },
	{ 0 },
	{ IR_REAR },
	{ 0 }
};
#else
ir_filter_t ir_filters[num_ir_sensors];
#endif

__rom const unsigned char ir_long_table_sigo2_in5[IR_LONG_TABLE_SIZE] = {
	180,180,180,180,180,180,180,180,180,180,180,180,
	180,180,180,180,180,180,180,180,180,180,180,177,
	173,168,163,159,156,154,151,147,141,138,136,134,
	133,131,127,122,119,117,115,114,112,110,106,103,
	100,99,97,96,95,94,93,92,91,89,87,86,84,82,81,79,
	78,78,77,76,75,74,73,72,71,70,69,69,68,67,67,66,
	65,64,64,63,62,61,61,60,59,58,58,57,56,56,55,54,
	53,53,52,51,50,50,50,49,49,49,48,48,48,47,47,47,
	46,46,46,45,45,45,44,44,44,43,43,43,42,42,42,41,
	41,41,40,40,40,39,39,39,39,38,38,38,38,37,37,37,
	37,36,36,36,36,35,35,35,35,34,34,34,34,33,33,33,
	33,32,32,32,32,31,31,31,30,30,30,30,30,30,30,29,
	29,29,29,29,29,29,29,28,28,28,28,28,28,28,28,27,
	27,27,27,27,27,27,27,27,26,26,26,26,26,26,26,26,
	25,25,25,25,25,25,25,25,25,24,24,24,24,24,24,24,
	24,23,23,23,23,23,23,23,23,23,22,22,22,22,22,22,
	22,22,21,21,21,21,21,21,21,21,20,20,20,20,20,20
};

__rom const unsigned char ir_short_table_sigo2_in5[IR_SHORT_TABLE_SIZE] = {
	120,120,120,120,120,120,120,120,120,120,120,120,
	116,111,98,95,92,89,84,80,77,74,71,69,68,67,66,
	65,64,63,62,61,60,58,55,53,51,50,49,48,48,47,46,
	46,45,44,44,43,43,42,41,41,40,40,39,39,38,38,37,
	37,37,36,36,35,35,34,34,34,33,33,32,32,31,31,31,
	30,30,30,30,29,29,29,29,29,29,28,28,28,28,28,28,
	27,27,27,27,27,27,26,26,26,26,26,26,25,25,25,25,
	25,25,24,24,24,24,24,24,23,23,23,23,23,23,22,22,
	22,22,22,22,21,21,21,21,21,21,20,20,20,20,20,20,
	20,20,20,20,20,19,19,19,19,19,19,19,19,19,19,19,
	19,19,19,19,18,18,18,18,18,18,18,18,18,18,18,18,
	18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
	14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,
	12,12,12,12,12
};

int IR_Long_To_In10(int sig)
{
	int index;
	
	index = sig/2;
	if (index >= IR_LONG_TABLE_SIZE) {
		return 0;
	}
	
	return (int)(ir_long_table_sigo2_in5[index]) * 2;
}

int IR_Short_To_In10(int sig)
{
	int index;
	
	index = sig/2;
	if (index >= IR_SHORT_TABLE_SIZE) {
		return 0;
	}
	
	return (int)(ir_short_table_sigo2_in5[index]) * 2;
}

// ----------------------------------------------------

void IR_Filter_Routine(void)
{
	uint8_t i;
	
	for (i=0; i<num_ir_sensors; i++) {
		ir_filters[i].s2 = ir_filters[i].s1;
		ir_filters[i].s1 = analog_adc_get(ir_filters[i].ana_port);
		
		if (ir_filters[i].s1 > ir_filters[i].s2) {
			ir_filters[i].output = ir_filters[i].s2;
		}
		else {
			ir_filters[i].output = ir_filters[i].s1;
		}
	}
}
