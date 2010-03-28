/******************** (C) COPYRIGHT 2009 IFI ********************
 * File Name: utilities.c
 * Purpose: Vex Slave utility code
 * Version: V1.00
 *----------------------------------------------------------------------------*/

#include "stm32f10x_lib.h"
#include "stdio.h"
#include "platform_config.h"

#define ADC1_DR_Address    ((u32) &ADC1->DR)
#define ADC3_DR_Address    ((u32)0x40013C4C)

unsigned char usartPort = 0;

extern void Set_MotorControl_To_Neutral(void);
extern vu16 ADCConvertedValue[];

static vu32 TimingDelay;

/* Private function prototypes -----------------------------------------------*/

/* Utility function prototypes -----------------------------------------------*/

void Setup_timer4(void) //************  Setup for Smart Motor Control
{                                                               
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  //TIM_OCInitTypeDef  TIM_OCInitStructure;
  /* ---------------------------------------------------------------
    TIM4 Configuration: Output Compare Toggle Mode:
    TIM2CLK = 72 MHz, Prescaler = 0x4, 0xFFFF = 4.5ms
  --------------------------------------------------------------- */

   AFIO->MAPR |= (1 << 12);    //Maps TIM4 to PORTD pins 12-15

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
}

void Setup_timer1(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* ---------------------------------------------------------------
    TIM1 Configuration: Output Compare Toggle Mode:
    TIM2CLK = 72 MHz, Prescaler = 20, 0xFFFF = 4.5ms
  --------------------------------------------------------------- */

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 20;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* TIM enable counter */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
  TIM1->SMCR &= 0xFFF8;
}

void Setup_Capture_Mode(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  GPIOB->CRH &= ~(0xF << 8);  //Maps PB10 to the Alternate Function
  GPIOB->CRH |= (0x4 << 8);   //Maps PB10 to the Alternate Function
  GPIOC->CRH &= ~0xF;         //Maps PC8 to the Alternate Function
  GPIOC->CRH |= 0x4;          //Maps PC8 to the Alternate Function

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 4;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM2->CCER = 0x000;   //disable capture on channel3, + polarity
  TIM2->ARR = 0xFFFF;
  TIM2->PSC = 16;       //Prescaler 72M/2/16+1 (each tick .05us)
  TIM2->CCMR2 = 0x001;  //Input capture 3 to channel 3 
  TIM2->CCMR2 |= (3 << 4);  // Input capture filter set to N=8 (250ns) 

  /* Generate an update event to reload the Prescaler value immediatly */
  TIM2->EGR = TIM_PSCReloadMode_Immediate;
  TIM2->DIER |= TIM_IT_CC3;
  TIM2->CCER = 0x100;   //enalbles capture on channel3, + polarity
  TIM_Cmd(TIM2, ENABLE);

  TIM3->CCER = 0x000;   //disable capture on channel3, + polarity
  TIM3->ARR = 0xFFFF;
  TIM3->PSC = 16;       //Prescaler 72M/2/16+1 (each tick .05us)
  TIM3->CCMR2 = 0x001;  // Input capture 3 to channel 3 
  TIM3->CCMR2 |= (3 << 4);  // Input capture filter set to N=8 (250ns) 

  /* Generate an update event to reload the Prescaler value immediatly */
  TIM3->EGR = TIM_PSCReloadMode_Immediate;
  TIM3->DIER |= TIM_IT_CC3;
  TIM3->CCER = 0x100;   //enalbles capture on channel3, + polarity
  TIM_Cmd(TIM3, ENABLE);
}

void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 115200;     // 115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  USART_Cmd(USARTx, ENABLE);

#ifdef USE_USART2  
  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
#endif
}

void SysTick_Configuration(void)
{
  // SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default)
  SysTick_SetReload(9000);
  // Enable SysTick interrupt
  SysTick_ITConfig(ENABLE);
  // Enable SysTick Counter
  SysTick_CounterCmd(SysTick_Counter_Enable);
}

void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    /* ADCCLK = PCLK2/4 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
    // Wait till PLL is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}
    // Select PLL as system clock source
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // Wait till PLL is used as system clock source
    while(RCC_GetSYSCLKSource() != 0x08){}
  }

#ifdef USE_USART1  //used for printf
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1 | 
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIO_LED | RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1 |
                         RCC_APB2Periph_TIM1, ENABLE);
#else //USE_USART2 used for Wifi Comms
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USARTx | RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1 | 
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIO_LED | RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1 |
                         RCC_APB2Periph_TIM1, ENABLE);
#endif
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  // Configure all unused GPIO port pins in Analog Input mode (floating input trigger OFF), 
  //this will reduce the power consumption and increase the device immunity against EMI/EMC
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_PinRemapConfig(GPIO_Remap_USART2 | GPIO_PartialRemap2_TIM2 | GPIO_PartialRemap_USART3 | 
                      GPIO_FullRemap_TIM3, ENABLE);

  /* Configure USART1_Tx */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Configure USART1_Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2_Tx */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* Configure USART2_Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

#if defined USE_TARGET_BOARD
  /* Configure GPIO_LED Pin 6, Pin 7, Pin 8 and Pin 9 as Output push-pull ----*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);

  /* Configure Key Button GPIO Pin as input floating (Key Button EXTI Line) */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_BUTTON;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_KEY_BUTTON, &GPIO_InitStructure);
#endif
   /* Configure PA.00 PA.03 as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure RTS ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* PB.10 as input floating RX1 pin*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

   /* Configure PC.00 PC.03 as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //************  Setup for Smart Motor Control
  Set_MotorControl_To_Neutral();  //Set pin prior to making them outputs

  /* PD.00, PD.01 as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIOD->CRH = (9 << 16) | (9 << 20) | (9 << 24) | ( 9 << 28) | 1;
  //************  End Setup for Smart Motor Control

  /* PD.00, PD.01 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* PE.13, PE.14 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* PE.09, PE.11, PE.13 and PE.14 as output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* 1st phase: SPI1 Master and SPI2 Slave */
  /* SPI1 Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //Highest Speed use 16
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  /* Enable SPI1 TXE interrupt */
  //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE | SPI_I2S_IT_TXE, ENABLE);
  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);
}

void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	// Enable DMA1 clock
	ADC_DeInit(ADC1);

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 8;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
     
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 8;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  2, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2,  3, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3,  4, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 7, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 8, ADC_SampleTime_239Cycles5);

  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  

  /* Enable the TIM1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel =  TIM1_CC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);

#ifdef USE_USART2  
  // Enable the USART1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Delay(u32 nTime)
{
  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
  
  TimingDelay = nTime;

  while(TimingDelay != 0);

  /* Disable SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Disable);
  /* Clear SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Clear);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef __GNUC__
  // With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar()
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  //#define GETCHAR_PROTOTYPE int __io_getchar()
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  //#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    // Write a character to the USART
    if(usartPort == 0)
        USART_SendData(USART1, (u8) ch);
    if(usartPort == 1)
        USART_SendData(USART2, (u8) ch);
    if(usartPort == 2)
        USART_SendData(USART3, (u8) ch);
    //USART_SendData(USARTx, (u8) ch);
    //USART_SendData((USART_TypeDef *)usartData[usartPort]._USARTx, (u8) ch);

    // Loop until the end of transmission
    if(usartPort == 0)
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
    else if(usartPort == 1)
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}
    else if(usartPort == 2)
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}
    //while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET){}
    //while(USART_GetFlagStatus((USART_TypeDef*)usartData[usartPort]._USARTx, USART_FLAG_TXE) == RESET){}
    
    return ch;
}


