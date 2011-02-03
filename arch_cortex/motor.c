
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
		p->gpio->CRL = (p->gpio->CRL & ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) << sh))
			| ((GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0
						| GPIO_CRH_CNF0_1) << sh);
	} else {
		uint8_t sh = 4 * (p->idx - 7);
		p->gpio->CRH = (p->gpio->CRH & ~((GPIO_CRH_MODE0 | GPIO_CRH_CNF0) << sh))
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
}

static void mtr_low_connect(struct mtr_side *ms)
{
	pin_set_low(ms->l);
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
	udelay(500);

	/* connect the high side */
	mtr_high_connect(ms);
}

static mtr_side_set_low(struct mtr_side *ms)
{
	/* disconnect the high side */
	mtr_high_discon(ms);

	/* recommended by Corey Chitwood of vex */
	udelay(500);

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

#define DEF_MOTOR_SET(x)		\
void motor##x##_set(motor_speed)	\
{}

static void timer4_init(void)
{
	/* Enable Clock */
	PERIPH_BIT_SET(RCC, APB1ENR, TIM4EN, 1);

	/* Remap TIM4 outputs.
	 * 0: No remap (TIM4_CH1/PB6, TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9)
	 * 1: Full remap (TIM4_CH1/PD12, TIM4_CH2/PD13, TIM4_CH3/PD14, TIM4_CH4/PD15)
	 * Note: TIM4_ETR on PE0 is not re-mapped.
	 */
	PERIPH_BIT_SET(AFIO, MAPR, TIM4_REMAP, 1);

	/* IFI:
	 * Output compare toggle mode
	 * TIM2CLK = 72 MHz, Prescaler = 0x4, period = 0xFFFF = 4.5ms
	 * ClocDivision = 0
	 * CounterMode = up
	 *
	 * ARR = 0x6E; // 1Khz chop rate
	 * PSC = 0x280; // Prescale rollover @ 4us
	 *
	 *
	 *
	 */

#if 0
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 4;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM4->CCMR1 = (6 << 12) | (6 << 4) | (1 << 11) | (1 << 3); //Pwm Mode 1
  TIM4->CCMR2 = (6 << 12) | (6 << 4) | (1 << 11) | (1 << 3); //Pwm Mode 1
  TIM4->ARR = 0x6E;   //1Khz chop rate
  TIM4->PSC = 0x280;  //Prescale rollover at 4us
  TIM4->CCER = (1 << 1) | (1 << 5) | (1 << 9) | (1 << 13) |  //Low side to active low
               (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);   //Turns on lowside outputs
  TIM4->CR1 = (1 << 0) | (1 << 7);  //Auto reload register is buffered

  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM4, ENABLE);

  /* TIM enable counter */
  TIM_Cmd(TIM4, ENABLE);

  /* TIM IT enable */
  //TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
#endif

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

