#include "stdbool.h"
#include "stdint.h"

static volatile uint32_t ct[6];
#define ENCODER(_flip_,_other_,_num_)              \
	do {                                       \
		bool other = digital_get(_other_); \
		if (_flip_) {                      \
			if (other)                 \
				ct[2*_num_+1]++;\
			else                       \
				ct[2*_num_]++;  \
		} else {                           \
			if (other)                 \
				ct[2*_num_]++;  \
			else                       \
				ct[2*_num_+1]++;\
		}                                  \
	} while (0)


void encoder_0a(int8_t l) {
	ENCODER( l,17,0); // 2
}
void encoder_0b(int8_t l) {
	ENCODER(!l,16,0); // 3
}
void encoder_1a(int8_t l) {
	ENCODER( l,19,1); // 4
}
void encoder_1b(int8_t l) {
	ENCODER(!l,18,1); // 5
}
void encoder_2a(int8_t l) {
	ENCODER( l,21,2); // 6
}
void encoder_2a(int8_t l) {
	ENCODER(!l,20,2); // 7
}

int32_t encoder_get(uint8_t n) {
	return (ct[2*n] - ct[2*n+1]);
}
