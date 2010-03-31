/******************** (C) COPYRIGHT 2009 IFI ********************
 * File Name: main.c
 * Purpose: Vex Slave template code
 * Version: V1.00
 *----------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL)
 //#define USE_STM3210E_EVAL
 #define USE_TARGET_BOARD
#endif

#define SPI_PACKET_SIZE      16

#define TBUF_SIZE   1024	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   1024      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

/* Define the STM32F10x hardware depending on the used evaluation board */
#define GPIO_LED                          GPIOF    
#define RCC_APB2Periph_GPIO_LED           RCC_APB2Periph_GPIOF
#define GPIO_KEY_BUTTON                   GPIOG    
#define RCC_APB2Periph_GPIO_KEY_BUTTON    RCC_APB2Periph_GPIOG
#define GPIO_PIN_KEY_BUTTON               GPIO_Pin_8
#define EXTI_LINE_KEY_BUTTON              EXTI_Line8
#define GPIO_PORT_SOURCE_KEY_BUTTON       GPIO_PortSourceGPIOG
#define GPIO_PIN_SOURCE_KEY_BUTTON        GPIO_PinSource8

//#define USE_USART1
#define USE_USART2  //For printf
//#define USE_UART4

#ifdef USE_USART1
  #define  USARTx                     USART1
  #define  GPIOx                      GPIOA
  #define  RCC_APB1Periph_USARTx      RCC_APB2Periph_USART1
  #define  GPIO_RxPin                 GPIO_Pin_10
  #define  GPIO_TxPin                 GPIO_Pin_9
#elif defined USE_USART2 && defined USE_TARGET_BOARD
  #define  USARTx                     USART2
  #define  RCC_APB1Periph_USARTx      RCC_APB1Periph_USART2
  #define  GPIOx                      GPIOD
  #define  RCC_APB2Periph_GPIOx       RCC_APB2Periph_GPIOD
  #define  GPIO_TxPin                 GPIO_Pin_5
  #define  GPIO_RxPin                 GPIO_Pin_6
#elif defined USE_USART2 && defined   USE_STM3210E_EVAL  
  #define  USARTx                     USART2
  #define  RCC_APB1Periph_USARTx      RCC_APB1Periph_USART2
  #define  GPIOx                      GPIOA
  #define  RCC_APB2Periph_GPIOx       RCC_APB2Periph_GPIOA
  #define  GPIO_TxPin                 GPIO_Pin_2
  #define  GPIO_RxPin                 GPIO_Pin_3
#elif defined USE_USART3
  #define  USARTx                     USART3
  #define  GPIOx                      GPIOB
  #define  RCC_APB2Periph_GPIOx       RCC_APB2Periph_GPIOB
  #define  RCC_APB1Periph_USARTx      RCC_APB1Periph_USART3
  #define  GPIO_RxPin                 GPIO_Pin_11
  #define  GPIO_TxPin                 GPIO_Pin_10
#elif defined USE_UART4
  #define  USARTx                     UART4
  #define  GPIOx                      GPIOC
  #define  RCC_APB2Periph_GPIOx       RCC_APB2Periph_GPIOC
  #define  RCC_APB1Periph_USARTx      RCC_APB1Periph_UART4
  #define  GPIO_RxPin                 GPIO_Pin_11
  #define  GPIO_TxPin                 GPIO_Pin_10
#elif defined USE_UART5
  #define  USARTx                     UART5
  #define  GPIOx                      GPIOC
  #define  RCC_APB2Periph_GPIOx       RCC_APB2Periph_GPIOC
  #define  RCC_APB1Periph_USARTx      RCC_APB1Periph_UART5
  #define  GPIO_RxPin                 GPIO_Pin_2
  #define  GPIO_TxPin                 GPIO_Pin_12
#endif

#define RX1_DTARDY      1    //pwmStatusFlags B0 data ready
#define RX2_DTARDY      2
#define RX1_INTRDY      4    //pwmStatusFlags B0 interrupt data ready 
#define RX2_INTRDY      8

//Joystick 1&2 Definitions
/*
  joystickx (Joystick1 - primary, Joystick2 - secondary (tethered)

  byte 0 = Axis 1        //Right Joystick
  byte 1 = Axis 2
  byte 2 = Axis 3        //Left Joystick
  byte 3 = Axis 4

  byte 4 = Axis x        //From Accelerometer
  byte 5 = Axis y
  byte 6 = Axis z

  byte 7 = Group 5 & 6   //Buttons  
   bit0 = Left Down       *** Group 5
   bit1 = Left Up
   bit2 = Right Down      *** Group 6
   bit3 = Left Down

  byte 8 = Group 7 & 8   //Buttons  
   bit0 = Down            *** Group 8
   bit1 = Left  
   bit2 = Up
   bit3 = Right
   bit4 = Down            *** Group 7
   bit5 = Left  
   bit6 = Up 
   bit7 = Right

  byte 9 = spare
  byte 10 = spare
  byte 11 = spare
*/


typedef struct  //Data From Master  ***Warning: This must not exceed 32 bytes
{ 
  u16 Sync;           //Sync Data = 0xC917  
  u8  State;           
  /*    
			Bit0 = Iack
			Bit1 = Config
			Bit2 = Intializing (data is not ready)
			Bit3 = Valid Data
  */
  u8  SystemFlags;      
  /*    
			Bit0 = TX 1 active when set
			Bit1 = TX 2 active when set
			Bit2 = Spare
			Bit3 = Competition Mode when set
			Bit4 = Reset Slave (Reserved)
			Bit5 = JoyStick Mode when set
			Bit6 = Autonomus when set
			Bit7 = Disable  when set
  */
  u8  mainBatteryVoltage;        
  u8  backupBatteryVoltage;     
  u8  joystick1[12];     
  u8  joystick2[12];    
  u8  version;    
  u8  packetNum;    
} Master_Spi_Record;

typedef struct  //Data To Master  ***Warning: This must not exceed 32 bytes
{ 
  u16 Sync;           //Sync Data = 0xC917  
  u8  State;           
  /*    
			Bit0 = Iack
			Bit1 = Config
			Bit2 = Intializing (data is not ready)
			Bit3 = Valid Data
  */
  u8  SystemFlags;    //Reserved for Slave (TBD)
  /*    
			Bit0 = Autonomous when =1, Operator Cntl when = 0 (with or w/o WiFi key)
			Bit1 = Cyrstal Mode when set (with or w/o WiFi key)
			Bit2 = Disable  when set
			Bit3 = Brake Mode  when set
			Bit4 = 1 turn printfs on   //Reserved for Master
			Bit5 = 1 easy display      //Reserved for Master
			Bit6 = 1 easy display 
  */
  u8  DigitalByte1;   //Digital bits 1-8      
  u8  DigitalByte2;   //Digital bits 9-12, 13-16 (spare)   
  u8  Motor[8];       //PWM values 0-255
  u8  MotorStatus[8]; //PWM motor states (TBD)
  u8  Analog[8];      //Analog port (1-8)
  u8  version;    
  u8  packetNum;    
} Slave_Spi_Record;

struct buf_st {
  unsigned int in;                                // Next In Index
  unsigned int out;                               // Next Out Index
  char buf [RBUF_SIZE];                           // Buffer
};

extern struct buf_st rbuf1;
extern struct buf_st tbuf1;
#define SIO_RBUF1LEN ((unsigned short)(rbuf1.in - rbuf1.out))
#define SIO_TBUF1LEN ((unsigned short)(tbuf1.in - tbuf1.out))

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);
void SysTick_Configuration(void);
void Delay(vu32 nTime);
void Setup_timer1(void);
void Setup_timer4(void);
void Setup_Capture_Mode(void);
void Handle_Configuration(void);
void Handle_Operator_Control(void);
void Handle_Functional_Test(void);
void Handle_Autonomous(void);
void Invoke_SPI_Transfer(void);
void WaitTillMasterIsReadyForSpiData(void);
void Handle_Data_From_Crystal(u8 rxPort);
void ADC_Config(void);
u16  Set_Digital_Out(u16 data);
u16  Set_Analog_Out(u16 data, u8 returnData);
u16  Set_Recvr_Out(u16 data);
u16  Set_Uart_Out(u16 data);
u16 Set_Bridge_Out(void);

/* Exported functions from ddt_support.c    --------------------------------- */
int GetKey (void);  
void PORTC_Toggle(unsigned char led);
void PORTD_Toggle(unsigned char led);
void PORTE_Toggle(unsigned char led);
void Handle_Debug_Input(unsigned char data);
void Process_Debug_Stream(void);

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
