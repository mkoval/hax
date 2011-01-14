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

int _write(int fd, const char *ptr, size_t len)
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

extern char _end; /* Defined by the linker */
static char *heap_end;

#if 0
static char *get_heap_end(void)
{
	return (char*) heap_end;
}

static char *get_stack_top(void)
{
	return (char*) __get_MSP();
}
#endif

#define DEBUG
#ifdef DEBUG
/* buf must be 32 / 4 = 8 bytes in length */
#define H32(i) (buf[i] = map[(q & (0xF << (32 / 4 - i)))] >> (32/4 - i))
void put_hex32(char *buf, uint32_t q)
{
	const char *map = "0123456789ABCDEF";
	H32(0);
	H32(1);
	H32(2);
	H32(3);
	H32(4);
	H32(5);
	H32(6);
	H32(7);
}
static char const msg1[] = "_sbrk : incr:";
static char const msg2[] = " heap_end:";
static char const msg3[] = " &end:";
static char const msg4[] = "\n";

static size_t const m1_len = sizeof(msg1)-1;
static size_t const m2_len = sizeof(msg2)-1;
static size_t const m3_len = sizeof(msg3)-1;
static size_t const m4_len = sizeof(msg4)-1;
#endif /* DEBUG */

caddr_t _sbrk(int incr)
{
#ifdef DEBUG
	char buf[8];
	_write(1, msg1, m1_len);
	put_hex32(buf, (uint32_t)incr);
	_write(1, buf, sizeof(buf));

	_write(1, msg2, m2_len);
	put_hex32(buf, (uint32_t)heap_end);
	_write(1, buf, sizeof(buf));

	_write(1, msg3, m3_len);
	put_hex32(buf, (uint32_t)&_end);
	_write(1, buf, sizeof(buf));

	_write(1, msg4, m4_len);
#endif
	void *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end;
	}

	prev_heap_end = heap_end;
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



// 1 means we are connected to a term.
// 0 means not ^.
int _isatty(int fd)
{
	return 1;
}
