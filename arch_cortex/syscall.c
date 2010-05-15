#include "stm32f10x.h"

#include <reent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "usart.h"

//#undef errno
//extern int errno;

int _getpid(void)
{
	return 1;
}

extern char _end; /* Defined by the linker */
static char *heap_end;

char* get_heap_end(void)
{
	return (char*) heap_end;
}

char* get_stack_top(void)
{
	return (char*) __get_MSP();
	// return (char*) __get_PSP();
}

caddr_t _sbrk(int incr)
{
	char *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
#if 0
	if (heap_end + incr > get_stack_top()) {
		xprintf("Heap and stack collision\n");
		abort();
	}
#endif
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _close(int fd)
{
	return -1;
}

int _fstat(int fd, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _lseek(int fd, int ptr, int dir) {
	return 0;
}

int _read(int fd, char *ptr, int len)
{
	return 0;
}

int _write(int fd, char *ptr, size_t len)
{
	size_t todo;
	
	for (todo = 0; todo < len; todo++) {
		if (fd == 0) {
			usart1_putc(*ptr);
		} else {
			usart1_putc(*ptr);
		}
		ptr++;
	}
	return len;
}

// 1 means we are connected to a term.
// 0 means not ^.
int _isatty(int fd)
{
	return 1;
}
