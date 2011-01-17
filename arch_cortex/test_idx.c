#include "arch.h"

#include <stdio.h>
#include <stdint.h>

typedef uint8_t index_t;

#define OI 1
#define GRP 3


void test_oi_group(uint8_t oi_i, uint8_t gr_i)
{
	index_t a = IX_OI_GROUP(oi_i, gr_i);

	uint8_t gr = IX_OI_GROUP_INV(a);
	uint8_t oi = IX_OI_OI_INV(a);

	printf("IN(%d, %d) == %d == OUT(%d, %d)\n",
			oi_i, gr_i, a, oi, gr);
}

void test_oi_button(uint8_t oi_i, uint8_t gr_i, uint8_t but_i)
{
	index_t a = IX_OI_BUTTON(oi_i, gr_i, but_i);

	int oi = IX_OI_BUTTON_OI_INV(a);
	int gr = IX_OI_BUTTON_GROUP_INV(a, oi);
	int but = -1;
	uint8_t b = IX_OI_BUTTON_INV_BUTTONS(a, oi);

	uint8_t mi = _IX_OI_BUTTON(gr_i, OI_B_UP);
	printf("IN(%d, %d, %d) == %d == OUT(%d, %d, %d) ~= %d ~= %d\n",
			oi_i, gr_i, but_i,
			a,
			oi, gr, but, b, mi);
}

int main(int argc, char **argv)
{
	{
		printf("IX_OI_GROUP & inverses\n");
		uint8_t oi, gr;
		for (oi = 1; oi <= 2; oi++) {
			for (gr = 1; gr <= CT_OI_GROUPS_PER_OI; gr++) {
				putchar('\t');
				test_oi_group(oi, gr);
			}
		}
	}

	{
		printf("IX_OI_BUTTON & inverses\n");
		uint8_t oi, gr, but;
		for (oi = 1; oi <= 2; oi++) {
			for (gr = 1; gr <= 8; gr++) {
				for (but = 0; but < 4; but++) {
					putchar('\t');
					test_oi_button(oi, gr,but);
				}
			}
		}
	}

	return 0;
}
