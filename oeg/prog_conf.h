#ifndef CONFIG_H_
#define CONFIG_H_

/* This header is included by other hax source files to
 * determine how the uC should be configured */
#include <stdio.h>


#define PROG_ANALOG_IN_NUM 16
#define WARN(fmt, ...) printf(fmt, ##__VA_ARGS__)

#endif
