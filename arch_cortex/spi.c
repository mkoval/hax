#include "stm32f10x.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

#include "vex_hw.h"
#include "spi.h"
#include "usart.h"
#include "compilers.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void spi_init(void)
{
	/* Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* GPIO */
	/*
	Alternate function SPI1_REMAP = 0 SPI1_REMAP = 1
	SPI1_NSS             PA4           PA15
	SPI1_SCK             PA5           PB3
	SPI1_MISO            PA6           PB4
	SPI1_MOSI            PA7           PB5
	*/

	GPIO_InitTypeDef GPIO_param;

	GPIO_param.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_param.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_param.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_param);

	GPIO_param.GPIO_Pin = GPIO_Pin_6;
	GPIO_param.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_param);

	/* SPI */
	SPI_InitTypeDef SPI_param;

	SPI_param.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_param.SPI_Mode = SPI_Mode_Master;
	SPI_param.SPI_DataSize = SPI_DataSize_16b;
	SPI_param.SPI_CPOL = SPI_CPOL_Low;
	SPI_param.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_param.SPI_NSS = SPI_NSS_Soft;
	SPI_param.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_param.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_param.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_param);

	SPI_Cmd(SPI1, ENABLE);

	/* Master Detect Lines: PE{3,4} */
	GPIO_param.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_param.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_param);

	/* Slave select : PE0 */
	GPIO_param.GPIO_Pin = GPIO_Pin_0;
	GPIO_param.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_param.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_param);

	/* Famed "RTS" Pin: PA11 */
	GPIO_param.GPIO_Pin = GPIO_Pin_11;
	GPIO_param.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_param.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_param);

	/* Master Connect Routines */
	uint8_t i;
	uint16_t init_data[8];

	// Slave Select?
	GPIO_SetBits(GPIOE, GPIO_Pin_0);

	// Interesting...
	for(i = 0; i < 8; i++)
		init_data[i] = SPI_I2S_ReceiveData(SPI1);
}

void print_oi(struct oi_data *oi)
{
	printf("rt1: %x; rt2: %x; "
		"lt1: %x; lt2: %x; "
		"aX: %x; aY: %x; aZ: %x; "
		,oi->axis_1
		,oi->axis_2
		,oi->axis_3
		,oi->axis_4
		,oi->accel_x
		,oi->accel_y
		,oi->accel_z
		);

	printf("g5( u:%x; d:%x); "
		"g6( u:%x; d:%x); "
		,oi->g5_u
		,oi->g5_d
		,oi->g6_u
		,oi->g6_d
		);

	printf("g8( d:%x; l:%x; "
		"u:%x; r%x );"
		,oi->g8_d
		,oi->g8_l
		,oi->g8_u
		,oi->g8_r
		);

	printf("g7( d:%x; l:%x; "
		"u:%x; r%x );"
		,oi->g7_d
		,oi->g7_l
		,oi->g7_u
		,oi->g7_r
		);
}

void print_m2u(spi_packet_m2u_t *m2u)
{
	printf("sync: %x; state: %x; "
		"sysflag: %x; b_main: %x; "
		"b_back: %x; version: %x; "
		"pnum: %x\n"
		,m2u->sync
		,m2u->state.a
		,m2u->sys_flags.a
		,m2u->batt_volt_main
		,m2u->batt_volt_backup
		,m2u->version
		,m2u->packet_num
		);
}

/** spi_valid - Dick Sawn of EasyC fame indicated in a forum post regarding
 * cortexes being "Burned out" by EasyC that the master, prior to setting the
 * valid bit in the SPI packet to 1, keeps all 4 low-side H-bridge lines
 * connected to ground.
 */
static bool spi_valid;
void spi_process_packets(spi_packet_m2u_t *m2u, spi_packet_u2m_t *u2m)
{
	/* Bad sync magic: bad packet. */
	if (m2u->sync != SYNC_MAGIC) {
		return;
	}

	if (m2u->state.b.config) {
		// config state
		static bool not_yet_iacked = 1;
		if (not_yet_iacked) {
			u2m->state.a = STATE_IACK | STATE_CONFIG;
		} else if (m2u->state.b.iack) {
			u2m->state.a = STATE_VALID;
		}
	}

	if (m2u->state.b.initializing) {
		// not yet good data.
		u2m->state.a = STATE_VALID; // we have data ready
		m2u->packet_num = 1; //XXX: "to skip print"
	}

	if (m2u->state.b.valid) {
		spi_valid = true;
		u2m->state.a = STATE_VALID;
		// TODO: Buffer the data.
	}
}

void vex_spi_xfer(spi_packet_m2u_t *m2u, spi_packet_u2m_t *u2m)
{
	static uint8_t packet_num = 0;
	uint8_t gap = 0;
	volatile uint16_t d = 0;
	uint8_t i = 0;
	spi_packet_t *u2m_raw = (spi_packet_t *)u2m;
	spi_packet_t *m2u_raw = (spi_packet_t *)m2u;

	u2m->packet_num = packet_num;

	GPIO_SetBits(GPIOA, GPIO_Pin_11); // "RTS" high

	for (i = 0; i < SPI_PACKET_LEN; i++) {
		GPIO_ResetBits(GPIOE, GPIO_Pin_0); // Slave Select
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI1, u2m_raw->w[i]);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		m2u_raw->w[i] = SPI_I2S_ReceiveData(SPI1);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		GPIO_SetBits(GPIOE, GPIO_Pin_0); // Slave Select
		for (d = 0; d < 150; d++); //XXX: Noted as "15us", truth?
		gap++;
		if (gap == 4) { //put a gap after 4 bytes xfered
			for (d = 0; d < 1000; d++); //210us
			GPIO_ResetBits(GPIOA, GPIO_Pin_11); //RTS low
			gap = 0;
		}
	}
	packet_num++;

	spi_process_packets(m2u,u2m);
}

bool is_master_ready(void)
{
	/* arch_init_2() blocks while this is false. */
	// master is ready when both input lines are low.
	return !GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) &&
		!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) &&
		spi_valid;
}

void spi_packet_init_m2u(spi_packet_m2u_t __unused *m2u)
{
}

void spi_packet_init_u2m(spi_packet_u2m_t *u2m)
{
	u2m->sync = SYNC_MAGIC;
	u2m->version = 1;
	u2m->packet_num = 0;

	// First send needs to be "config"
	u2m->state.a = STATE_CONFIG;

	uint8_t i;
	for(i = 0; i < MOTOR_CT; i++) {
		u2m->motors[i] = 127;
	}
}


