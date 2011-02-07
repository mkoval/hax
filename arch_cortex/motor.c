
#include <stdint.h>
#include <stm32f10x.h>

#include "motor.h"

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
	volatile GPIO_TypeDef *gpio;
	uint8_t idx;
};

struct mtr_side {
	struct pin h;
	struct pin l;
};

/* do initial gpio init prior to bringing up TMR4 */
static void pin_setup_output_pp_50(struct pin *p)
{
	/* MODE = 11 = 50MHz output
	 * CNF  = 00 = Push-pull output
	 */
	if (p->idx =< 7) {
		uint8_t sh = 4 * p->idx;
		p->gpio->CRL = (p->gpio->CRL & ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) << sh))
			| ((GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0) << sh);
	} else {
		uint8_t sh = 4 * (p->idx - 7);
		p->gpio->CRH = (p->gpio->CRH & ~((GPIO_CRH_MODE0 | GPIO_CRH_CNF0) << sh))
			| ((GPIO_CRH_MODE0_1 | GPIO_CRH_MODE0_0) << sh);
	}
}

static void pin_setup_af_pp_50(struct pin *p)
{
	/* MODE = 11 = 50MHz output
	 * CNF  = 10 = Alternate function push-pull output
	 */
	if (p->idx =< 7) {
		uint8_t sh = 4 * p->idx;
		p->gpio->CRL = (p->gpio->CRL &
				~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) << sh))
				| ((GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0
						| GPIO_CRH_CNF0_1) << sh);
	} else {
		uint8_t sh = 4 * (p->idx - 7);
		p->gpio->CRH = (p->gpio->CRH &
				~((GPIO_CRH_MODE0 | GPIO_CRH_CNF0) << sh))
				| ((GPIO_CRH_MODE0_1 | GPIO_CRH_MODE0_0
						| GPIO_CRH_CNF0_1) << sh);
	}
}


static void pin_set_low(struct pin *p)
{
	p->gpio->BSR = (1 << p->idx);
}

static void pin_set_high(struct pin *p)
{
	p->gpio->BSSR = (1 << p->idx);
}

static void mtr_low_discon(struct mtr_side *ms)
{
	pin_set_high(ms->l);
	pin_setup_output_pp_50(ms->l)
}

static void mtr_low_connect(struct mtr_side *ms)
{
	/* pin_set_low(ms->l); */
	pin_setup_af_pp_50(ms->l)
}

static void mtr_high_connect(struct mtr_side *ms)
{
	pin_set_high(ms->h);
}

static void mtr_high_discon(struct mtr_side *ms)
{
	pin_set_low(ms->h);
}

static void mtr_side_discon(struct mtr_side *ms)
{
	/* disconnect the high end */
	mtr_high_discon(ms);

	/* disconnect the low end */
	mtr_low_discon(ms);
}

/* setup the pins under the control of GPIO */
static void mtr_side_setup(struct mtr_side *ms)
{
	mtr_side_discon(ms);

	/* set pins as Out_PP, 50MHz */
	pin_setup_output_pp_50(ms->h);
	pin_setup_output_pp_50(ms->l);
}

/* hand over control of the high pins to tmr4 */
static void mtr_side_tmr_ctrl(struct mtr_side *ms)
{
	pin_setup_af_pp_50(ms->h);
	pin_setup_af_pp_50(ms->l);
}

static mtr_side_set_high(struct mtr_side *ms)
{
	/* disconnect the low side */
	mtr_low_discon(ms);

	/* recommended by Corey Chitwood of vex */
	udelay_500();

	/* connect the high side */
	mtr_high_connect(ms);
}

static mtr_side_set_low(struct mtr_side *ms)
{
	/* disconnect the high side */
	mtr_high_discon(ms);

	/* recommended by Corey Chitwood of vex */
	udelay_500();

	/* connect the low side */
	mtr_low_connect(ms);
}

struct motor {
	struct mtr_side a;
	struct mtr_side b;
} static m_data [] = {
	/*   AH          AL               BH          BL */
	{ { {GPIOD, 3}, {GPIOD, 12} }, { {GPIOD, 4}, {GPIOD, 13} } },
	{ { {GPIOD, 7}, {GPIOD, 14} }, { {GPIOD, 8}, {GPIOD, 15} } }
}

#define DEF_MOTOR_SET(x, an, bn)         \
void motor##x##_set(int16_t motor_speed) \
{                                        \
	if (motor_speed > 0) {               \
		TIM4->CR##bn = 0;                \
		mtr_high_discon(m_data[x].a);    \
		mtr_low_discon(m_data[x].b);     \
		udelay_500();                    \
		TIM4->CR##an = motor_speed;      \
		mtr_low_connect(m_data[x].a);    \
		mtr_high_connect(m_data[x].b);   \
	} else if (motor_speed < 0) {        \
		TIM4->CR##an = 0;                \
		mtr_high_discon(m_data[x].b);    \
		mtr_low_discon(m_data[x].a);     \
		udelay_500();                    \
		TIM4->CR##bn = -motor_speed;     \
		mtr_low_connect(m_data[x].b);    \
		mtr_high_connect(m_data[x].a);   \
	} else {                             \
		TIM4->CR##an = 0;                \
		TIM4->CR##bn = 0;                \
		mtr_high_discon(m_data[x].a);    \
		mtr_high_discon(m_data[x].b);    \
		udelay_500();                    \
		mtr_low_connect(m_data[x].a);    \
		mtr_low_connect(m_data[x].b);    \
	}                                    \
}

DEF_MOTOR_SET(0, 1, 2);
DEF_MOTOR_SET(1, 3, 4);


/* timer4_init - sets up TIM4 to handle control of low-side of h-bridges.
 *
 * this timer is used because none of the others could access the pins.
 */
static void timer4_init(void)
{
	/* Enable Clock */
	PERIPH_BIT_SET(RCC, APB1ENR, TIM4EN, 1);

	/* Clears the TIM_CR1_CEN bit, disabling timer. Sets the
	 * center aligned mode to 3: update events on both up and
	 * down overflows. */
	TIM4->CR1 = TIM_CR1_CMS_1 | TIM_CR1_CMS0;

	/* Remap TIM4 outputs.
	 * 0: No remap (TIM4_CH1/PB6, TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9)
	 * 1: Full remap (TIM4_CH1/PD12, TIM4_CH2/PD13, TIM4_CH3/PD14,
	 *			TIM4_CH4/PD15)
	 * Note: TIM4_ETR on PE0 is not re-mapped.
	 */
	PERIPH_BIT_SET(AFIO, MAPR, TIM4_REMAP, 1);


	/* TOP = 0x7FFF */
	TIM4->ARR = INT16_MAX;

	TIM4->CCR1 = 0;
	TIM4->CCR2 = 0;
	TIM4->CCR3 = 0;
	TIM4->CCR4 = 0;

	/* PWM mode 1, load CCR on update event. */
	TIM4->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE
	            | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
	TIM4->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE
	            | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE;

	/* Enable outputs, Active low */
	TIM4->CCER = TIM_CCER_CC1P | TIM_CCER_CC1E
	           | TIM_CCER_CC2P | TIM_CCER_CC2E
		   | TIM_CCER_CC3P | TIM_CCER_CC3E
		   | TIM_CCER_CC4P | TIM_CCER_CC4E;

	/* Prescale
	 * 72*1000*1000/0x7fff = 2197Hz */
	TIM4->PSC = 0;

	/* Enable */
	TIM4->CR1 |= TIM_CR1_CEN;
}

#define mtr_setup_1(m) do {	\
	mtr_side_setup(m.a);	\
	mtr_side_setup(m.b);	\
} while(0)

#define mtr_setup_2(m) do {	\
	mtr_side_tmr_ctrl(m.a);	\
	mtr_side_tmr_ctrl(m.b);	\
} while(0)

void motors_init(void)
{
	mtr_setup_1(m_data[0]);
	mtr_setup_1(m_data[1]);

	/* setup timer for PWM */
	timer4_init();

	mtr_setup_2(m_data[0]);
	mtr_setup_2(m_data[1]);
}

