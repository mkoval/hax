#ifndef SPI_H_
#define SPI_H_

#include "vex_hw.h"
#include <stdbool.h>

// prints the packet from the master
void print_m2u(spi_packet_m2u_t *m2u);

// prints joystick data.
void print_oi(struct oi_data *oi);

// spi packet initialization. expects zeroed packets.
void spi_packet_init_m2u(spi_packet_m2u_t *m2u);
void spi_packet_init_u2m(spi_packet_u2m_t *u2m);

// on startup,
bool is_master_ready(void);

void vex_spi_xfer(spi_packet_m2u_t *m2u, spi_packet_u2m_t *u2m);
void vex_spi_process_packets(spi_packet_m2u_t *m2u, spi_packet_u2m_t *u2m);

void spi_init(void);

#endif
