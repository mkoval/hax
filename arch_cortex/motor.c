
#include <stdint.h>
#include <stm32f10x.h>

/*        VBatt
 *       -------
 *       |     |
 *   AH /       \ BH
 *       |     |
 *       ---M---
 *       |     |
 *  !AL /       \ !BL
 *       |     |
 *       -------
 *         0V
 */

/*
 * AH1  PD3
 * AL1~ PD12
 * BH1  PD4
 * BL1~ PD13
 * AH2  PD7
 * AL2~ PD14
 * BH2  PD8
 * BL2~ PD15
 */

/* posible timers to utilize: */
struct pin {
	GPIO_TypeDef *port;
	uint8_t idx;
};

struct mtr_side {
	struct pin h;
	struct pin l;
};

static void pin_setup_output(struct pin *p)
{
	/* TODO */
}

static void pin_set_low(struct pin *p)
{

}

static void pin_set_high(struct pin *p)
{

}

static void mtr_side_discon(struct mtr_side *ms)
{
	pin_set_low(ms->h);
	pin_set_high(ms->l);
}

static void mtr_side_setup(struct mtr_side *ms)
{

}

static mtr_side_set_high(struct mtr_side *ms)
{
	/* disconnect the low side */
	pin_set_high(ms->l);

	/* recommended by Corey Chitwood of vex */
	udelay(500);

	/* connect the high side */
	pin_set_high(ms->h);
}

static mtr_side_set_low(struct mtr_side *ms)
{
	/* disconnect the high side */
	pin_set_low(ms->h);

	/* recommended by Corey Chitwood of vex */
	udelay(500);

	/* connect the low side */
	pin_set_low(ms->l);
}

struct motor {
	struct mtr_side a;
	struct mtr_side b;
} static m_data [] = {
	/*   AH          AL               BH          BL */
	{ { {GPIOD, 3}, {GPIOD, 12} }, { {GPIOD, 4}, {GPIOD, 13} } },
	{ { {GPIOD, 7}, {GPIOD, 14} }, { {GPIOD, 8}, {GPIOD, 15} } }
}

#define M_CT ARRAY_SIZE(m_data)

#define DEF_MOTOR_SET(x)		\
void motor##x##_set(motor_speed)	\
{}

void motors_init(void)
{
	/* setup timer for PWM */
	timer_pwm_setup();

	uint_fast8_t i;
	for (i = 0; i < M_CT; i++) {
		struct motor *m = m_data + i;
		mtr_side_setup(&m->a);
		mtr_side_setup(&m->b);
	}
}

