#ifndef PROG_CONF_H_
#define PROG_CONF_H_

#include <ports.h>
#define PROG_ANALOG_IN_NUM ANA_NUM

#define WARN(...) _WARN(__LINE__,__VA_ARGS__)

#include <stdio.h>
#define _WARN(line, ...) __WARN(line, __VA_ARGS__)
#define __WARN(line, ...) do {                  \
	fputs(__FILE__ ":" #line ":", stdout);  \
	fputs(__FUNCTION__, stdout);            \
	fputs("() : ", stdout);                 \
	printf(__VA_ARGS__);                    \
	putchar('\n');                          \
} while(0)

#endif
