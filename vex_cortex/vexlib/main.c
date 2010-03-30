/******************** (C) COPYRIGHT 2009 IFI ********************
 * File Name: main.c
 * Purpose: Vex Slave template code
 * Version: V1.00
 *
 * The Slave is setup to be a SPI Master.  An interrupt (Timer 1) occurs every
 * 18ms.  The Timer 1 interrupt handler sets a flag (updateMaster) every 18ms.  
 * This flag enables the slave to send SPI data to the Master while receiving
 * data from the Master during the SPI transfer.  
 *
 * Data from Wifi port (via the orange cable) comes in on Uart1.  Data sent to Uart1
 * goes back to the PC. 
 *
 * Timers Used:
 * TMR1 is used to update Master at 18ms rate 
 * TMR2 is used for RX1 capture
 * TMR3 is used for RX2 capture
 * TMR4 is used for Smart Motor Control
 * PB.10 as input RX1 pin
 * PC.08 as input RX2 pin
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "stm32f10x_lib.h"
#include "stdio.h"
#include "platform_config.h"

#define SYNC_MAGIC 0xC917


#define SET_ATON_WITH_KEY       0	//Slave invoked Autonomous Mode

#define MAX_MOTORS              8

#define MOTOR2                  slavePtr->Motor[0]
#define MOTOR3                  slavePtr->Motor[1]
#define MOTOR4                  slavePtr->Motor[2]
#define MOTOR5                  slavePtr->Motor[3]
#define MOTOR6                  slavePtr->Motor[4]
#define MOTOR7                  slavePtr->Motor[5]
#define MOTOR8                  slavePtr->Motor[6]
#define MOTOR9                  slavePtr->Motor[7]

#define JOY1_CHAN1              masterPtr->joystick1[0]
#define JOY1_CHAN2              masterPtr->joystick1[1]
#define JOY1_CHAN3              masterPtr->joystick1[2]
#define JOY1_CHAN4              masterPtr->joystick1[3]
#define JOY2_CHAN1              masterPtr->joystick2[0]
#define JOY2_CHAN2              masterPtr->joystick2[1]
#define JOY2_CHAN3              masterPtr->joystick2[2]
#define JOY2_CHAN4              masterPtr->joystick2[3]

#define BufferSize              32
u16 SPI1_Buffer_Rx[BufferSize], SPI1_Buffer_Tx[BufferSize];

Slave_Spi_Record *slavePtr;	//Used to update Master with data from Slave
Master_Spi_Record *masterPtr;	//Used to receive data from Master
u8 dataFromPC = 0;
u8 rx1Timeout = 0;
u8 rx2Timeout = 0;
u8 systemStateFlags = 0;
static u8 crystalActive = 0;
static u8 autonTestFlag = 0;
static u8 ConfigurationMode;
static u8 mot1Dir, mot2Dir, mot3Dir, mot4Dir, mot5Dir;
static u8 mot6Dir, mot7Dir, mot8Dir, mot9Dir, mot10Dir;
vu16 ADCConvertedValue[10];
int analogMask;

u16 rx1[BufferSize];
u16 rx2[BufferSize];

extern u16 pwm1[];
extern u16 pwm2[];
extern u8 pwmStatusFlags;
extern u8 updateMaster;
extern u8 SendDataToUart;

extern void Debug_Send(void);

void Set_MotorControl_To_Neutral(void);
void Set_MotorControl_Sw2(u8 data);

int main(void)
{
	u16 j, i;
	int uart1data = uart1data;	//to ignore compile warning
	u8 delay = 0;
	float b1, m2;
	u8 sendit = 0;

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

	slavePtr = (Slave_Spi_Record *) & SPI1_Buffer_Tx[0];
	masterPtr = (Master_Spi_Record *) & SPI1_Buffer_Rx[0];
	memset(slavePtr, 0, sizeof(Slave_Spi_Record));

	slavePtr->Sync = SYNC_MAGIC;
	slavePtr->State = 2;	//Send Configuration to Master
	slavePtr->version = 1;	//Slave Version Number
#if SET_ATON_WITH_KEY
	slavePtr->SystemFlags = 1;	//Set Slave Induced Autonomous Mode with Wifi Key
#else
	slavePtr->SystemFlags = 0;	//Let Master control Modes
#endif
	printf("Version 2\r\n");
	Set_MotorControl_To_Neutral();
	for (i = 0; i < MAX_MOTORS; i++)	//Update Motors
	{
		slavePtr->Motor[i] = 127;
		ADCConvertedValue[i] = 100;
	}
	for (i = 0; i < BufferSize; i++)	//Initialize crystal buffers
	{
		rx1[i] = 127;
		rx2[i] = 127;
	}
	analogMask = 0;

	while (1) {
		//PORTD_Toggle(0);  //Dig11
#ifdef USE_USART2
		uart1data = GetKey();	//Any data from Wifi port?
		if (uart1data >= 0)
			Handle_Debug_Input(uart1data);	//If so, parse DDT data
		Process_Debug_Stream();
#endif
		if ((pwmStatusFlags & RX1_INTRDY) == RX1_INTRDY)	//Int Data from RX1 ready?
			Handle_Data_From_Crystal(1);
		if ((pwmStatusFlags & RX2_INTRDY) == RX2_INTRDY)	//Int Data from RX2 ready?
			Handle_Data_From_Crystal(2);

		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10))	//RX1 Connected?
			crystalActive |= 1;
		if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))	//RX2 Connected
			crystalActive |= 2;

#if SET_ATON_WITH_KEY
#else
		if (crystalActive) {
			slavePtr->SystemFlags = 2;	//Set Slave Crystal Autonomous Mode 
			if ((pwmStatusFlags & 3) == 0)	//RX1 or RX2 not valid?
				slavePtr->SystemFlags |= 4;	//disable
			else
				slavePtr->SystemFlags &= ~4;	//enable
		} else {
			slavePtr->SystemFlags = autonTestFlag;	//Let Master control Modes
		}
#endif

		if (updateMaster)	//Timer 4 sets this flag every 20ms
		{
			if ((sendit == 0) && (SendDataToUart))
				Debug_Send();
			sendit = (sendit + 1) % 6;

			if ((systemStateFlags & 0x20) == 0x20)
				printf("rcvr %x tmo1 %d tmo2 %d\r\n",
				       pwmStatusFlags, rx1Timeout,
				       rx2Timeout);
			updateMaster = 0;

			//if (delay == 0) 
			//PORTC_Toggle(7);  //Dig4
			delay = (delay + 1) % 10;

			if (rx1Timeout)
				rx1Timeout--;
			else {
				pwmStatusFlags &= ~RX1_DTARDY;	//Timeout on RX1 occured
				crystalActive &= ~1;
			}
			if (rx2Timeout)
				rx2Timeout--;
			else {
				pwmStatusFlags &= ~RX2_DTARDY;	//Timeout on RX2 occured
				crystalActive &= ~2;
			}

			Invoke_SPI_Transfer();

			if (masterPtr->Sync == SYNC_MAGIG )	//Valid Sync?
			{
				if ((masterPtr->State & 2) == 2)	//Config data?
				{
					Handle_Configuration();
				} else if ((masterPtr->State & 4) == 4)	//Waiting for Initialization to complete
				{
					printf
					    ("Waiting for Valid Data from Master st %x\r\n",
					     masterPtr->State);
					slavePtr->State = 8;	//Indicate that Slave has data Ready
					masterPtr->packetNum = 1;	//To skip print
				} else if (masterPtr->State == 8)	//Valid data?
				{
					slavePtr->State = 8;	//Slave data is now ready
					if (((masterPtr->SystemFlags & 0x40) == 0x40) ||	//Master Induced Autonomous Mode
					    (slavePtr->SystemFlags == 1))	//Slave Induced Autonomous Mode
					{
						Handle_Autonomous();
					} else {
						Handle_Operator_Control();
					}
				}
			}
#if 1				// print master data on UART2
			systemStateFlags = 0x9;
			if (((systemStateFlags & 1) == 1) && ((masterPtr->packetNum % 5) == 0))	//Display SPI data at a slower rate
			{
				if ((systemStateFlags & 8) == 8) {
					b1 = masterPtr->
					    backupBatteryVoltage * 0.0591;
					m2 = masterPtr->
					    mainBatteryVoltage * 0.0591;
#if 1
					for (j = 0; j < MAX_MOTORS; j++)
						slavePtr->Analog[j] =
						    ADCConvertedValue[j] >>
						    2;
					printf("%x:",
					       masterPtr->SystemFlags);
					for (j = 0; j < 4; j++)
						printf("%02x ",
						       masterPtr->
						       joystick1[j]);
					//printf(", %x %x %x %x %x %x %x %x %x %x",mot1Dir,mot2Dir,mot3Dir,mot4Dir,mot5Dir,mot6Dir,mot7Dir,mot8Dir,mot9Dir,mot10Dir);
					printf(": %x %x :",
					       ConfigurationMode,
					       analogMask);
					for (j = 0; j < MAX_MOTORS; j++)
						printf("%02x ",
						       slavePtr->
						       Analog[j]);
#else
					if (crystalActive) {
						printf("M %x S %x:",
						       masterPtr->
						       SystemFlags,
						       slavePtr->
						       SystemFlags);
						for (j = 0; j < MAX_MOTORS;
						     j++)
							printf("%02x ",
							       slavePtr->
							       Motor[j]);
					} else {
						printf("%x:",
						       masterPtr->
						       SystemFlags);
						for (j = 0; j < 9; j++)
							printf("%02x ",
							       masterPtr->
							       joystick1
							       [j]);
						printf(",");
						for (j = 0; j < 9; j++)
							printf("%02x ",
							       masterPtr->
							       joystick2
							       [j]);
					}
#endif
					//printf("st %x ",masterPtr->SystemFlags);
				} else if ((systemStateFlags & 2) == 0) {
					for (j = 1;
					     j < SPI_PACKET_SIZE - 1; j++)
						printf("%02x",
						       SPI1_Buffer_Rx[j]);
					printf(" %02x %02x",
					       masterPtr->version,
					       masterPtr->packetNum);
					slavePtr->DigitalByte2 = 1;
				} else {
					if ((systemStateFlags & 4) == 0) {
						for (j = 0; j < 8; j++)
							printf("%04x",
							       SPI1_Buffer_Rx
							       [j]);
						slavePtr->DigitalByte2 = 2;
					} else {
						for (j = 0; j < 8; j++)
							printf("%04x",
							       SPI1_Buffer_Rx
							       [j + 8]);
						slavePtr->DigitalByte2 = 3;
					}
				}
				printf("\r");
			}
#endif
		}		//if (updateMaster)
		//Delay(200);
	}			// while (1)
}

/* Support function prototypes -----------------------------------------------*/

void Handle_Configuration(void)
{
	static u8 OneShot = 1;

	if (OneShot)		//Send Config Iack only one time
	{
		slavePtr->State = 3;	//Send Config data | Iack
	} else if ((masterPtr->State & 1) == 1)	//Did the Master IACK?
	{
		slavePtr->State = 8;	//Indicate that Slave is ready for Data
	}
	OneShot = 0;
	printf("MS %x, SS %x\r\n", masterPtr->State, slavePtr->State);
}

void Set_MotorControl_To_Neutral(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	//AH1
	GPIO_ResetBits(GPIOD, GPIO_Pin_4);	//BH1 
	GPIO_ResetBits(GPIOD, GPIO_Pin_7);	//AH2 
	GPIO_ResetBits(GPIOD, GPIO_Pin_8);	//BH2 

	GPIO_SetBits(GPIOD, GPIO_Pin_12);	//~AL1 
	GPIO_SetBits(GPIOD, GPIO_Pin_13);	//~BL1
	GPIO_SetBits(GPIOD, GPIO_Pin_14);	//~AL2 
	GPIO_SetBits(GPIOD, GPIO_Pin_15);	//~BL2 
}

void Set_MotorControl_Sw1(u8 data)
{
	u8 newValue;

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

unsigned char Limit_Mix(int intermediate_value)
{
	static int limited_value;

	if (intermediate_value < 2000) {
		limited_value = 2000;
	} else if (intermediate_value > 2254) {
		limited_value = 2254;
	} else {
		limited_value = intermediate_value;
	}
	return (unsigned char) (limited_value - 2000);
}

void Set_MotorControl_Sw2(u8 data)
{
	u8 newValue;

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

u8 Set_Direction(u8 direction, u8 value)
{
	u8 motor = value;

	if (direction)
		motor = 255 - value;

	return motor;
}

void Handle_Group5_6(u8 butnData)
{
	if ((butnData & 1) == 1)	//Group 5 Down
		MOTOR6 = 0;
	else if ((butnData & 2) == 2)	//Up
		MOTOR6 = 255;
	else
		MOTOR6 = 127;
	MOTOR6 = Set_Direction(mot6Dir, MOTOR6);

	if ((butnData & 4) == 4)	//Group 6 Down
		MOTOR7 = 0;
	else if ((butnData & 8) == 8)	//Up
		MOTOR7 = 255;
	else
		MOTOR7 = 127;
	MOTOR7 = Set_Direction(mot7Dir, MOTOR7);
}

void Handle_Group7_8(u8 butnData)
{
	if ((butnData & 0x10) == 0x10)	//Group 7 Down
		MOTOR8 = 0;
	else if ((butnData & 0x40) == 0x40)	//Up
		MOTOR8 = 255;
	else
		MOTOR8 = 127;
	MOTOR8 = Set_Direction(mot8Dir, MOTOR8);

	if ((butnData & 0x1) == 0x1)	//Group 8 Down
		MOTOR9 = 0;
	else if ((butnData & 0x4) == 0x4)	//Up
		MOTOR9 = 255;
	else
		MOTOR9 = 127;
	MOTOR9 = Set_Direction(mot9Dir, MOTOR9);
}

void One_Stick(u8 direction, u8 x1, u8 x2, u8 * m1, u8 * m2)
{
	u8 xrev;

	if (direction) {
		xrev = x1;
		*m1 = Limit_Mix(2000 + xrev + x2 - 127);	/* LEFT  WHEELS */
		*m2 = Limit_Mix(2000 + x2 - xrev + 127);	/* RIGHT WHEELS */
	} else {
		xrev = 255 - x1;
		*m1 = Limit_Mix(2000 + xrev + x2 - 127);	/* LEFT  WHEELS */
		*m2 = Limit_Mix(2000 + x2 - xrev + 127);	/* RIGHT WHEELS */
		*m1 = 255 - *m1;	/* reverse direction of left side */
	}
}

#define CLOSED        1

void Limit_Switch_Max(u16 switch_state, u8 * input_value)
{
	if (switch_state == CLOSED) {
		if (*input_value > 127)
			*input_value = 127;
	}
}

void Limit_Switch_Min(u16 switch_state, u8 * input_value)
{
	if (switch_state == CLOSED) {
		if (*input_value < 127)
			*input_value = 127;
	}
}

u8 Adjust_Motor(u8 value)
{
	u8 motor = value;

	if ((value) && (value != 255))
		motor--;

	return motor;
}

void Driver_Mode(void)
{
	MOTOR2 = Set_Direction(!mot2Dir, JOY1_CHAN3);
	MOTOR3 = Set_Direction(!mot3Dir, JOY1_CHAN3);
	MOTOR4 = Set_Direction(mot4Dir, JOY1_CHAN2);
	MOTOR5 = Set_Direction(mot5Dir, JOY1_CHAN2);
	Set_MotorControl_Sw1(Set_Direction(mot1Dir, JOY1_CHAN3));	//MOTOR1
	Set_MotorControl_Sw2(Set_Direction(!mot10Dir, JOY1_CHAN2));	//MOTOR10
}

void Get_Motor_Directions()	//Get direction from digital inputs 1-10
{
	mot1Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9);	//Dig1 
	mot2Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11);	//Dig2 
	mot3Dir = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);	//Dig3 
	mot4Dir = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);	//Dig4 
	mot5Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13);	//Dig5 
	mot6Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14);	//Dig6 
	mot7Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8);	//Dig7 
	mot8Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10);	//Dig8 
	mot9Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12);	//Dig9 
	mot10Dir = !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7);	//Dig10
}

void Get_Analog_Mask()		//Build mask from A2D channels 1-8 
{
	int i;

	analogMask = 0;
	for (i = 0; i < MAX_MOTORS; i++) {
		if (ADCConvertedValue[i] < 10)	//If a jumper is placed on an analog port, the app LED will light up
			analogMask |= 1 << i;
	}
}

void Get_Configuration_Mode()	//Get Config mode from digital inputs 11-12
{
	ConfigurationMode = 0;
	if (!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0))	//Dig11
	{
		ConfigurationMode |= 1;
		GPIO_SetBits(GPIOE, GPIO_Pin_11);	//DIG2
	}
	if (!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1))	//Dig12
	{
		ConfigurationMode |= 2;
		GPIO_SetBits(GPIOC, GPIO_Pin_6);	//DIG3
	}
}

void Handle_Operator_Control()
{
	int i;
	u8 *ptr;
	u8 motorL, motorR;

	//if ((masterPtr->SystemFlags & 0x03) == 0) && (crystalActive == 0) return;  //Crystals?

	Get_Configuration_Mode();
	Get_Motor_Directions();
	Get_Analog_Mask();
	ptr = &masterPtr->joystick1[0];

	for (i = 0; i < MAX_MOTORS; i++)	//Update Motors
	{
		if (dataFromPC == 0)	//not using DDT?
		{
			if (i == 6)
				ptr = &masterPtr->joystick2[0];
			slavePtr->Motor[i] = *ptr++;
		}
		if ((pwmStatusFlags & RX1_DTARDY) == RX1_DTARDY)	//if data from RX1 then update Motors
			slavePtr->Motor[i] = rx1[i + 1];
		if ((pwmStatusFlags & RX2_DTARDY) == RX2_DTARDY)	//if data from RX2 then update Motors
			slavePtr->Motor[i] = rx2[i + 1];

		if (dataFromPC == 0)	//not using DDT?
			slavePtr->MotorStatus[i] = 0;
		slavePtr->Analog[i] = 0;
	}
	if (crystalActive == 0) {
		switch (ConfigurationMode) {
		case 0:	//Single Driver Tank
			Driver_Mode();	//MOTOR2,MOTOR3,MOTOR4,MOTOR5
			Handle_Group5_6(masterPtr->joystick1[7]);	//MOTOR6, MOTOR7
			Handle_Group7_8(masterPtr->joystick1[8]);	//MOTOR8, MOTOR9
			break;
		case 1:	//Dual Driver Tank
			Driver_Mode();	//MOTOR2,MOTOR3,MOTOR4,MOTOR5
			Handle_Group5_6(masterPtr->joystick2[7]);	//MOTOR6, MOTOR7
			MOTOR8 = Set_Direction(!mot8Dir, JOY2_CHAN3);
			MOTOR9 = Set_Direction(!mot9Dir, JOY2_CHAN2);
			break;
		case 2:	//Single Driver Arcade
			One_Stick(1, JOY1_CHAN2, JOY1_CHAN1, &motorR,
				  &motorL);
			MOTOR2 = Set_Direction(mot2Dir, motorL);
			MOTOR3 = Set_Direction(mot3Dir, motorL);
			MOTOR4 = Set_Direction(mot4Dir, motorR);
			MOTOR5 = Set_Direction(mot5Dir, motorR);
			motorL = Set_Direction(1, Adjust_Motor(motorL));
			motorR = Set_Direction(1, Adjust_Motor(motorR));
			Set_MotorControl_Sw1(Set_Direction(mot1Dir, motorL));	//MOTOR1
			Set_MotorControl_Sw2(Set_Direction(mot10Dir, motorR));	//MOTOR10
			Handle_Group5_6(masterPtr->joystick1[7]);	//MOTOR6, MOTOR7
			MOTOR8 = Set_Direction(!mot8Dir, JOY1_CHAN3);
			MOTOR9 = Set_Direction(!mot9Dir, JOY1_CHAN4);
			break;
		case 3:	//Dual Driver Arcade
			One_Stick(1, JOY1_CHAN2, JOY1_CHAN1, &motorR,
				  &motorL);
			MOTOR2 = Set_Direction(mot2Dir, motorL);
			MOTOR3 = Set_Direction(mot3Dir, motorL);
			MOTOR4 = Set_Direction(mot4Dir, motorR);
			MOTOR5 = Set_Direction(mot5Dir, motorR);
			motorL = Set_Direction(1, Adjust_Motor(motorL));
			motorR = Set_Direction(1, Adjust_Motor(motorR));
			Set_MotorControl_Sw1(Set_Direction(mot1Dir, motorL));	//MOTOR1
			Set_MotorControl_Sw2(Set_Direction(mot10Dir, motorR));	//MOTOR10
			Handle_Group5_6(masterPtr->joystick2[7]);	//MOTOR6, MOTOR7
			MOTOR8 = Set_Direction(!mot8Dir, JOY2_CHAN3);
			MOTOR9 = Set_Direction(!mot9Dir, JOY2_CHAN2);
			break;
		}
	}
	Limit_Switch_Min(((analogMask & 0x01) == 0x01), &MOTOR6);
	Limit_Switch_Max(((analogMask & 0x02) == 0x02), &MOTOR6);
	Limit_Switch_Min(((analogMask & 0x04) == 0x04), &MOTOR7);
	Limit_Switch_Max(((analogMask & 0x08) == 0x08), &MOTOR7);
	Limit_Switch_Min(((analogMask & 0x10) == 0x10), &MOTOR8);
	Limit_Switch_Max(((analogMask & 0x20) == 0x20), &MOTOR8);
	Limit_Switch_Min(((analogMask & 0x40) == 0x40), &MOTOR9);
	Limit_Switch_Max(((analogMask & 0x80) == 0x80), &MOTOR9);
}

void Handle_Autonomous(void)
{
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

void Invoke_SPI_Transfer(void)
{
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

void WaitTillMasterIsReadyForSpiData(void)
{
	int temp = temp;
	int j;
	int masterNotReady = TRUE;
	int delay = 0;

	GPIO_SetBits(GPIOE, GPIO_Pin_0);	//SPI1_INT
	for (j = 0; j < 8; j++)
		temp = SPI_I2S_ReceiveData(SPI1);
	for (j = 0; j < SPI_PACKET_SIZE; j++)
		SPI1_Buffer_Tx[j] = 127;

	printf("Vex Slave Waiting for Master\r\n");
	while (masterNotReady)	//Loop Here until Master is ready to handle SPI data
	{
		Delay(200);
		if (!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3))	//PROG_SEL1
			if (!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4))	//PROG_SEL2
				masterNotReady = FALSE;
		delay++;
		if (delay > 50) {
			delay = 0;
			if (masterNotReady)
				printf("Waiting ...\r\n");
		}
	}
	printf("Wait Done ...\r\n");
}

void Handle_Data_From_Crystal(u8 receiverPort)
{
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
		//if ((systemStateFlags & 8) == 8) printf("%d ",rxPtr[i]);
		fPWM = (float) pwmPtr[i] / 14.0;
		rxPtr[i] = (u16) fPWM;
		if (rxPtr[i] > 255)
			rxPtr[i] = 255;
		if ((rxPtr[i] >= 123) && (rxPtr[i] <= 131))
			rxPtr[i] = 127;
		//if ((systemStateFlags & 0x10) == 0x10) printf("%d ",rxPtr[i]);
	}
	//if ((systemStateFlags & 8) == 8) printf("\r\n");
	//if ((systemStateFlags & 0x10) == 0x10) printf("\r\n");
}
