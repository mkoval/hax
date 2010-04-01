#include <hax.h>
#include <stdbool.h>
#include <string.h>

#include "fwlib/inc/stm32f10x_spi.h"
#include "fwlib/inc/stm32f10x_type.h"
#include "fwlib/inc/stm32f10x_gpio.h"
#include "vexlib/platform_config.h"

/*
 * VEXLIB IMPLEMENTATION
 */

/* Temporary constants. */
#define MAX_MOTORS 8
#define BufferSize 32

Slave_Spi_Record  *slavePtr;
Master_Spi_Record *masterPtr;

static uint8_t crystalActive = 0;

uint16_t SPI1_Buffer_Rx[BufferSize];
uint16_t SPI1_Buffer_Tx[BufferSize];

uint16_t rx1[BufferSize];
uint16_t rx2[BufferSize];

uint8_t dataFromPC = 0;
uint8_t sendit = 0;
uint8_t rx1Timeout = 0;
uint8_t rx2Timeout = 0;
uint8_t systemStateFlags = 0;

int analogMask; /* TODO Use a explicitly sized data type. */

volatile uint16_t ADCConvertedValue[10];

/* Definitions that should be in the Vex headers. */
extern u16 pwm1[];
extern u16 pwm2[];
extern uint8_t pwmStatusFlags;
extern uint8_t SendDataToUart;
extern uint8_t updateMaster;

void Debug_Send(void);

void Handle_Data_From_Crystal(u8 receiverPort) {
	u8 i, start;
	u16 *pwmPtr;
	u16 *rxPtr;
	float fPWM;

	//crystalActive |= receiverPort;
	if (receiverPort == 1)	//RX1?
	{
		pwmPtr = &pwm1[0];
		rxPtr = &rx1[0];
		pwmStatusFlags &= ~RX1_INTRDY;
		pwmStatusFlags |= RX1_DTARDY;
		rx1Timeout = 5;	//(5 * 18ms)
	} else			//must be RX2
	{
		pwmPtr = &pwm2[0];
		rxPtr = &rx2[0];
		pwmStatusFlags &= ~RX2_INTRDY;
		pwmStatusFlags |= RX2_DTARDY;
		rx2Timeout = 5;	//(5 * 18ms)
	}
	start = 1;
	if ((systemStateFlags & 8) == 8)
		start = 0;
	for (i = start; i < 7; i++) {
		rxPtr[i] = pwmPtr[i];
		fPWM = (float) pwmPtr[i] / 14.0;
		rxPtr[i] = (u16) fPWM;
		if (rxPtr[i] > 255)
			rxPtr[i] = 255;
		if ((rxPtr[i] >= 123) && (rxPtr[i] <= 131))
			rxPtr[i] = 127;
	}
}

void Invoke_SPI_Transfer(void) {
	static u8 packetNum = 0;
	int i, j, gap;

	GPIO_SetBits(GPIOA, GPIO_Pin_11);	//RTS high Used to ensure 1st 4 bytes
	gap = 0;
	for (j = 0; j < SPI_PACKET_SIZE; j++) {
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);	//Use as SSL0
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) ==
		       RESET);
		SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[j]);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) ==
		       RESET);
		SPI1_Buffer_Rx[j] = SPI_I2S_ReceiveData(SPI1);

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) ==
		       RESET);
		GPIO_SetBits(GPIOE, GPIO_Pin_0);	//Use as SSL0
		for (i = 0; i < 150; i++);	//15us
		gap++;
		if (gap == 4)	//put a gap after 4 bytes xfered  
		{
			for (i = 0; i < 1000; i++);	//210us
			GPIO_ResetBits(GPIOA, GPIO_Pin_11);	//RTS low
			gap = 0;
		}
	}
	slavePtr->packetNum = packetNum++;
}

void WaitTillMasterIsReadyForSpiData(void) {
	int temp = temp;
	int j;
	int masterPtrNotReady = true;
	int delay = 0;

	GPIO_SetBits(GPIOE, GPIO_Pin_0);	//SPI1_INT
	for (j = 0; j < 8; j++)
		temp = SPI_I2S_ReceiveData(SPI1);
	for (j = 0; j < SPI_PACKET_SIZE; j++)
		SPI1_Buffer_Tx[j] = 127;

	while (masterPtrNotReady)	//Loop Here until Master is ready to handle SPI data
	{
		Delay(200);
		if (!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3))	//PROG_SEL1
			if (!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))	//PROG_SEL2
				masterPtrNotReady = false;
		delay++;
		if (delay > 50) {
			delay = 0;
		}
	}
}

void Set_MotorControl_Sw1(uint8_t data) {
	uint8_t newValue;

	if (data > 129) {
		TIM4->CCR2 = 0;
		GPIO_ResetBits(GPIOD, GPIO_Pin_3);	//AH1 
		newValue = data - 128;
		GPIO_SetBits(GPIOD, GPIO_Pin_4);	//BH1 
		TIM4->CCR1 = newValue;
	} else if (data < 126) {
		TIM4->CCR1 = 0;
		GPIO_ResetBits(GPIOD, GPIO_Pin_4);	//BH1 
		newValue = 128 - data;
		GPIO_SetBits(GPIOD, GPIO_Pin_3);	//AH1 
		TIM4->CCR2 = newValue;
	} else {
		TIM4->CCR1 = 0;
		TIM4->CCR2 = 0;
		GPIO_ResetBits(GPIOD, GPIO_Pin_3);	//AH1
		GPIO_ResetBits(GPIOD, GPIO_Pin_4);	//BH1 
	}
}

void Set_MotorControl_Sw2(u8 data) {
	uint8_t newValue;

	if (data > 129) {
		TIM4->CCR4 = 0;
		GPIO_ResetBits(GPIOD, GPIO_Pin_7);	//AH2 
		newValue = data - 128;
		GPIO_SetBits(GPIOD, GPIO_Pin_8);	//BH2 
		TIM4->CCR3 = newValue;
	} else if (data < 126) {
		TIM4->CCR3 = 0;
		GPIO_ResetBits(GPIOD, GPIO_Pin_8);	//BH2 
		newValue = 128 - data;
		GPIO_SetBits(GPIOD, GPIO_Pin_7);	//AH2 
		TIM4->CCR4 = newValue;
	} else {
		TIM4->CCR3 = 0;
		TIM4->CCR4 = 0;
		GPIO_ResetBits(GPIOD, GPIO_Pin_7);	//AH2 
		GPIO_ResetBits(GPIOD, GPIO_Pin_8);	//BH2 
	}
}

void Handle_Autonomous(void) {
	int i;

	for (i = 0; i < MAX_MOTORS; i++)	//Update Motors
	{
		if (dataFromPC == 0)	//not using DDT?
			slavePtr->Motor[i] = ADCConvertedValue[i] >> 2;
		slavePtr->MotorStatus[i] = 0;
		slavePtr->Analog[i] = 0;
	}
	Set_MotorControl_Sw1(slavePtr->Motor[0]);
	Set_MotorControl_Sw2(slavePtr->Motor[1]);
}

void Set_MotorControl_To_Neutral(void) {
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	//AH1
	GPIO_ResetBits(GPIOD, GPIO_Pin_4);	//BH1 
	GPIO_ResetBits(GPIOD, GPIO_Pin_7);	//AH2 
	GPIO_ResetBits(GPIOD, GPIO_Pin_8);	//BH2 

	GPIO_SetBits(GPIOD, GPIO_Pin_12);	//~AL1 
	GPIO_SetBits(GPIOD, GPIO_Pin_13);	//~BL1
	GPIO_SetBits(GPIOD, GPIO_Pin_14);	//~AL2 
	GPIO_SetBits(GPIOD, GPIO_Pin_15);	//~BL2 
}

void Handle_Operator_Control() {
	/* TODO Implement this. */
}

/*
 * INITIALIZATION AND MISC
 */
void setup_1(void) {
	AnalogOutIx i;
	int uart1data = uart1data; /* Hack to get around a GCC error. */

	/* TODO Reimplement these functions. */
	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	ADC_Config();
	USART_Configuration();
	SysTick_Configuration();
	Setup_Capture_Mode();
	Setup_timer1();
	Setup_timer4();

	/* Set the location of the interrupt vector to 0x08000000. */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	WaitTillMasterIsReadyForSpiData(); 

	/* Initialize communication with the masterPtr processor. */
	slavePtr  = (Slave_Spi_Record *)  &SPI1_Buffer_Tx[0];
	masterPtr = (Master_Spi_Record *) &SPI1_Buffer_Rx[0];
	memset(slavePtr, 0, sizeof(Slave_Spi_Record));

	slavePtr->Sync         = 0xC917;
	slavePtr->State        = 2; /* Bitmask; config flag. */
	slavePtr->SystemFlags  = 0; /* Let the masterPtr processor control modes. */
	slavePtr->DigitalByte1 = 0;
	slavePtr->DigitalByte2 = 0;
	slavePtr->version      = 1; /* Slave version number. */
	slavePtr->packetNum    = 0;

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
		slavePtr->Motor[i]      = 127;
		ADCConvertedValue[i] = 100; /* ? */
	}

	/* Initialize crystal buffers. */
	for (i = 0; i < BufferSize; ++i) {
		rx1[i] = 127;
		rx2[i] = 127;
	}

	analogMask = 0; /* ? */
}

void setup_2(void) {
}

void loop_1(void) {

	/* Handle data from RX1 if it is connected. */
	if (pwmStatusFlags & RX1_INTRDY) {
		Handle_Data_From_Crystal(1);
	}

	/* Handle data from RX2 if it is connected. */
	if (pwmStatusFlags & RX2_INTRDY) {
		Handle_Data_From_Crystal(2);
	}

	if (!sendit && SendDataToUart) {
		Debug_Send(); /* ? */
	}

	if (rx1Timeout > 0) {
		--rx1Timeout;
	}
	/* Timeout waiting for data from RX1. */
	else {
		pwmStatusFlags &= ~RX1_DTARDY;
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

	/* Invoke the SPI transfer between the masterPtr and slavePtr. */
	Invoke_SPI_Transfer();

	/* Valid sync with the masterPtr processor. */
	if (masterPtr->Sync == 0xC917) {
		/* Handle masterPtr-slavePtr configuration. */
		if (masterPtr->State & 2) {
			/* TODO Replace this state machine with something in init. */

			/* Send the configuration IACK once. */
			if (one_shot) {
				slavePtr->State = 3;
			}
			/* If the masterPtr IACK's, the slavePtr is ready for data. */
			else if (masterPtr->State & 1) {
				slavePtr->State = 8;
			}
			one_shot = 0;
		}
		/* Wait for an instruction to complete. */
		else if (masterPtr->State & 4) {
			slavePtr->State = 8;      /* Slave has data ready. */
			masterPtr->packetNum = 1; /* ? */
		}
	}
}

void loop_2(void) {
}

void spin(void) {
	static uint8_t one_shot = 1;
	int uart1data = uart1data; /* Hack to get around a GCC error. */

#ifdef USE_USART2
	/* Check for data transmitted over wifi. */
	uart1data = GetKey();

	if (uart1data >= 0) {
		Handle_Debug_Input(uart1data);
	}

	Process_Debug_Stream();
#endif
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
	if (masterPtr->SystemFlags & 0x40 || slavePtr->SystemFlags == 1) {
		return kModeAuton;
	} else {
		return kModeTelop;
	}
	/* TODO What about disabled mode? */
}

void mode_set(CtrlMode mode) {
	/* TODO Implement this by modifying the slave's SystemFlags, if it is
	 * possible to safely do so.
	 */
}


/*
 * ANALOG AND DIGITAL INPUTS
 */
/* Mapping between an external pin number and the collection of registers that
 * it uses for configuration and data storage.
 */
static GPIO_TypeDef *const gpio_reg[] = {
	GPIOE, GPIOE, GPIOC, GPIOC, GPIOE, GPIOE,
	GPIOE, GPIOE, GPIOE, GPIOE, GPIOD, GPIOD
};

static uint8_t const gpio_rep[] = {
	0, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1
}

static uint8_t const gpio_pin[] = {
	9, 11, 6, 7, 13, 14, 8, 10, 12, 7, 0, 1
};

static uint8_t const gpio_num = 12;

void pin_set_io(PinIx index, PinMode mode) {
}

int8_t analog_oi_get(OIIx);
uint16_t analog_adc_get(PinIx);

void digital_set(PinIx, bool);

bool digital_get(PinIx index) {
	/* TODO Pin may need to be configured to get a successful read. */

	/* The i-th bit of a GPIO IDR register is the value of the i-th digital pin
	 * stored in that register.
	 */
	if (index < gpio_num) {
		return !!(gpio_reg[index]->IDR & (1 << gpio_pin[index]));
	} else {
		return false;
	}
}

/*
 * MOTOR AND SERVO OUTPUTS
 */
void analog_set(AnalogOutIx index, AnalogOut sp) {
	sp = (sp < 0 && sp != -128) ? sp - 1 : sp;
	slavePtr->motor[index] = sp + 128;
}

void motor_set(AnalogOutIx index, MotorSpeed sp) {
	analog_set(index, sp);
}

void servo_set(AnalogOutIx index, ServoPosition pos) {
	analog_set(index, pos);
}

/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */
InterruptServiceRoutine isr_callbacks[20] = { 0 };

void interrupt_reg_isr(InterruptIx index, InterruptServiceRoutine isr) {
	isr_callbacks[index] = isr;

	NVIC_Init
}

bool interrupt_get(InterruptIx index) {
	return digital_get(index);
}

void interrupt_set(InterruptIx, bool) {
}

void interrupt_enable(InterruptIx);
void interrupt_disable(InterruptIx);

