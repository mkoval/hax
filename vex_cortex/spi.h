#ifndef SPI_H_
#define SPI_H_

#include "vex_hw.h"
#include <stdbool.h>

extern volatile bool spi_transfer_flag;

// prints the packet from the master
void print_m2u(spi_packet_vex *m2u);

// prints joystick data.
void print_oi(struct oi_data *oi);

// spi packet initialization. expects zeroed packets.
void spi_packet_init_m2u(spi_packet_vex *m2u);
void spi_packet_init_u2m(spi_packet_vex *u2m);

// on startup, 
bool is_master_ready(void);

void vex_spi_xfer(spi_packet_vex *m2u, spi_packet_vex *u2m);
void vex_spi_process_packets(spi_packet_vex *m2u, spi_packet_vex *u2m);

void spi_init(void);

#endif
