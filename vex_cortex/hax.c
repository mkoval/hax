#include <hax.h>
#include <stdbool.h>

static Slave_Spi_Record  *slave;
static Master_Spi_Record *master;

void setup_1(void) {
	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	ADC_Config();
	USART_Configuration();
	SysTick_Configuration();
	Setup_Capture_Mode();
	Setup_timer1();
	Setup_timer4();

	WaitTillMasterIsReadyForSpiData();

	s
}

void setup_2(void) {
}

void spin(void) {
}

void loop_1(void) {
}

void loop_2(void) {
}

CtrlMode mode_get(void) {
	return 0;
}

bool new_data_received(void) {
	return false;
}

/*
 * INITIALIZATION AND MISC
 */
void setup_1(void) {
	AnalogOutIx i, j;
	uint8_t delay, sendit;
	int uart1data = uart1data;  //to ignore compile warning
	float b1, m2;

	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	ADC_Config();
	USART_Configuration();
	SysTick_Configuration();
	Setup_Capture_Mode();
	Setup_timer1();
	Setup_timer4(); /* TODO Implement this timer... */

	WaitTillMasterIsReadyForSpiData(); 

	/* Initialize communication with the master processor. */
	slave  = (Slave_Spi_Record *)  &SPI1_Buffer_Tx[0];
	master = (Master_Spi_Record *) &SPI1_Buffer_Rx[0];
	memset(slave, 0, sizeof(Slave_Spi_Record));

	slave->Sync         = 0xC917;
	slave->State        = 2; /* Bitmask; config flag. */
	slave->SystemFlags  = 0; /* Let the master processor control modes. */
	slave->DigitalByte1 = 0;
	slave->DigitalByte2 = 0;
	slave->version      = 1; /* Slave version number. */
	slave->packetNum    = 0;

	/* Set motor control to neutral. */
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);
	GPIO_ResetBits(GPIOD, GPIO_Pin_4);
	GPIO_ResetBits(GPIOD, GPIO_Pin_7);
	GPIO_ResetBits(GPIOD, GPIO_Pin_8);

	GPIO_SetBits(GPIOD, GPIO_Pin_12);
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
	GPIO_SetBits(GPIOD, GPIO_Pin_14);
	GPIO_SetBits(GPIOD, GPIO_Pin_15);

	/* Initialize the motors to neutral. */
	for (i = 0; i < MAX_MOTORS; ++i) {
		slave->Motor[i]      = 127;
		ADCConvertedValue[i] = 100; /* ? */
	}

	/* Initialize crystal buffers. */
	for (i = 0; i < BufferSize; ++i) {
		rx1[i] = 127;
		rx2[i] = 127;
	}

	analogMask = 0; /* ? */
}

void setup_2(void);

void spin(void) {
#ifdef USE_USART2
	/* Check for data transmitted over wifi. */
	uart1data = GetKey();

	if (uart1data >= 0) {
		Handle_Debug_Input(uart1data);
	}

	Process_Debug_Stream();
#endif

	/* Handle data from RX1 if it is connected. */
	if (pwmStatusFlags & RX1_INTRDY) {
		Handle_Data_From_Crystal(1);
	}

	/* Handle data from RX2 if it is connected. */
	if (pwmStatusFlags & RX2_INTRDY) {
		Handle_Data_From_Crystal(2);
	}
}

void loop_1(void) {
	static uint8_t one_shot = 1;

	if (!sendit && SendDataToUart()) {
		Debug_Send(); /* ? */
	}

	if (rx1Timeout > 0) {
		--rx1Timeout;
	}
	/* Timeout waiting for data from RX1. */
	else {
		pwmStatusFlags &= ~RX_DTARDY;
		crystalActive  &= ~1;
	}

	if (rx2Timeout > 0) {
		--rx2Timeout;
	}
	/* Timeout waiting for data from RX2. */
	else {
		pwmStatusFlags &= ~RX2_DTARDY;
		crystalActive  &= ~2;
	}

	/* Invoke the SPI transfer between the master and slave. */
	GPIO_SetBits(GPIOA, GPIO_Pin_11); /* ? */

	for (i = 0; i < SPI_PACKET_SIZE; ++i) {
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[j]);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		SPI1_Buffer_Rx[j] = SPI_I2S_ReceiveData(SPI1);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		GPIO_SetBits(GPIOE, GPIO_Pin_0);

		/* Wait for 15 microseconds. */
		/* TODO: Replace this with something less hacky; maybe noops? */
		for (i = 0; i < 150; ++i);

		

	Invoke_SPI_Transfer();

	/* Valid sync with the master processor. */
	if (master->Sync == 0xC917) {
		/* Handle master-slave configuration. */
		if (master->State & 2) {
			/* TODO Replace this state machine with something in init. */

			/* Send the configuration IACK once. */
			if (one_shot) {
				slave->State = 3;
			}
			/* If the master IACK's, the slave is ready for data. */
			else if (master->State & 1) {
				slave->State = 8;
			}
			one_shot = 0;
		}
		/* Wait for an instruction to complete. */
		else if (master->State & 4) {
			slave->State = 8;      /* Slave has data ready. */
			master->packetNum = 1; /* ? */
		}
		/* Receied valid data; update autonomous mode. */
		else if (master->State == 8) {
			/* Autonomous is either forced by master or was set by slave. */
			if (master->SystemFlags & 0x40 || slave->SystemFlags == 1) {
				Handle_Autonomous();
			} else {
				Handle_Operator_Control();
			}
		}
	}
}

void loop_2(void) {
}

bool new_data_received(void) {
	/* This flag is set to true by timer 4 every 20ms. */
	if (updateMaster) {
		updateMaster = false;
		return true;
	}
	return false;
}

CtrlMode mode_get(void) {
}

void mode_set(CtrlMode) {
}

