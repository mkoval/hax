/*
 * INTERNAL FUNCTIONS
 */
void hw_setup1(void) {
	rcc_init();
	gpio_init();
	usart_init();
	spi_init();
	nvic_init();
	tim1_init();
	adc_init();
	exti_init();

	memset(&u2m, 0, sizeof u2m);
	memset(&m2u, 0, sizeof m2u);

	spi_packet_init_u2m(&u2m);
	spi_packet_init_m2u(&m2u);

	printf("[ INIT DONE ]\n");
}

void hw_setup2(void) {
	while(!is_master_ready());
}

void hw_spin(void) {
}

void hw_loop1(void) {
	vex_spi_xfer(&m2u, &u2m);
	spi_transfer_flag = false;
}

void hw_loop2(void) {
}

bool hw_ready(void) {
	return spi_transfer_flag;
}

mode_t mode_get(void) {
	if (m2u.m2u.sys_flags.b.autonomus) {
		return MODE_AUTON;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return MODE_DISABLE;
	} else {
		return MODE_TELOP;
	}
}
