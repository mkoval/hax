/*----------------------------------------------------------------------------
 * Name:    ddt_support.c
 * Purpose: DDT usage for the IFI Loader
 * Version: V1.00
 *----------------------------------------------------------------------------*/

#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include <stdio.h>
#include "stm32f10x_it.h"
#include "platform_config.h"

#define READ_CMD            0           // Dynamic Debug commands           
#define READ_PWMs           1           // Reads PWM data
#define READ_rxSPI          2           // Used for reading rxdata
#define READ_txSPI          3           // Used for reading txdata
#define UPDATE_PWM_MASK     7           // Used for updating the PWM Mask
#define READ_EE_DATA        8           // Used for reading EE data

#define WRITE_CMD           20
#define WRITE_PWMs          21          // Updates PWM data
#define WRITE_rxSPI         22          // Used for updating rxdata
#define WRITE_txSPI         23          // Used for updating txdata
#define WRITE_EE_DATA       25          // Used for writing EE data

/*******************************************************************************
* This stucture is used to pass data from the PC to the device driver.
*******************************************************************************/
typedef struct
{ 
  unsigned char  RX_SYNC1;    //Always 0xAA
  unsigned char  RX_SYNC2;    //Always 0x55
  unsigned char  RX_INDEX;     
  unsigned char  RX_DATAH;
  unsigned char  RX_DATAL;
  unsigned char  RX_CMD;
  unsigned char  RX_FSRH;
  unsigned char  RX_FSRL;
} DYNO_INPUT_RECORD;

/*******************************************************************************
* This stucture is used to pass data from the device driver to the PC.
*******************************************************************************/
typedef struct
{ 
  unsigned char  TX_SYNC1;    //Always 0xAA
  unsigned char  TX_SYNC2;    //Always 0x55
  unsigned char  TX_INDEX;     
  unsigned char  TX_DATAH;
  unsigned char  TX_DATAL;
  unsigned char  TX_CMD;
} DYNO_OUTPUT_RECORD;

static unsigned char debugIntState;   /* used for incomming data from PC */
static DYNO_OUTPUT_RECORD dataOut;    /* data output buffer (to PC) */
static DYNO_INPUT_RECORD  dataIn;     /* data input buffer (from PC) */
u8 SendDataToUart = 0;

extern unsigned char dataFromPC;      /* used for incomming data from PC */
extern Slave_Spi_Record *slavePtr;
extern unsigned int tx_restart1;  
extern u8 systemStateFlags;

#if DEMO_BOARD
void LED_Show(unsigned char led)
{
  static unsigned long  LedCounter = 0;
  u16 gpio;

  gpio = 1 << led;
  LedCounter ^= (1 << led);
  if (LedCounter & (1 << led))
    GPIO_SetBits(GPIOF, gpio);
  else 
    GPIO_ResetBits(GPIOF, gpio);
}
#endif

int GetKey (void) {
  struct buf_st *p = &rbuf1;

  if (SIO_RBUF1LEN == 0)
    return (-1);

  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}

void PORTC_Toggle(unsigned char led)
{
  static unsigned long  LedCounter = 0;
  u16 gpio;

  gpio = 1 << led;
  LedCounter ^= (1 << led);
  if (LedCounter & (1 << led))
    GPIO_SetBits(GPIOC, gpio);
  else 
    GPIO_ResetBits(GPIOC, gpio);
}

void PORTD_Toggle(unsigned char led)
{
  static unsigned long  LedCounter = 0;
  u16 gpio;

  gpio = 1 << led;
  LedCounter ^= (1 << led);
  if (LedCounter & (1 << led))
    GPIO_SetBits(GPIOD, gpio);
  else 
    GPIO_ResetBits(GPIOD, gpio);
}

void PORTE_Toggle(unsigned char led)
{
  static unsigned long  LedCounter = 0;
  u16 gpio;

  gpio = 1 << led;
  LedCounter ^= (1 << led);
  if (LedCounter & (1 << led))
    GPIO_SetBits(GPIOE, gpio);
  else 
    GPIO_ResetBits(GPIOE, gpio);
}

int SendCharUart1 (int c) {
  struct buf_st *p = &tbuf1;
                                                 // If the buffer is full, return an error value
  if (SIO_TBUF1LEN >= TBUF_SIZE)
    return (-1);
                                                  
  p->buf [p->in & (TBUF_SIZE - 1)] = c;           // Add data to the transmit buffer.
  p->in++;

  if (tx_restart1) {                               // If transmit interrupt is disabled, enable it
    tx_restart1 = 0;
   	USART1->CR1 |= USART_FLAG_TXE;		          // enable TX interrupt
  }
  return (0);
}

#if 1
void Debug_Send(void)
{
  u16 i;
  u8 data;

  dataOut.TX_SYNC1 = 0xaa;
  dataOut.TX_SYNC2 = 0x55;
  data = 0x30;
  for (i=0;i<512;i++)
  {
    data++;
    if (data > 250) data = 0x30;
    SendCharUart1(data);
  }
}
#else
void Debug_Send(void)
{
  u8 i,*ptr;

  dataOut.TX_SYNC1 = 0xaa;
  dataOut.TX_SYNC2 = 0x55;
  ptr = (unsigned char *)&dataOut;
  for (i=0;i<6;i++)
  {
    SendCharUart1(ptr[i]);
  }
}
#endif
void Handle_Debug_Input(unsigned char data) //Using IFI's DDT 
{
  switch (debugIntState)
  {
    case 0:  // 1st 0xAA
      if (data == 0xC9) debugIntState = 1;
      break;
    case 1 : // 2nd 0x55
      debugIntState = 0;
      if (data == 0x17) debugIntState = 2;
      break;
    case 2 : //get idx
      dataIn.RX_INDEX = data;
      debugIntState = 3;
      break;
    case 3 : //get dataH
      dataIn.RX_DATAH = data;
      debugIntState = 4;
      break;
    case 4 : //get dataL
      dataIn.RX_DATAL = data;
      debugIntState = 5;
      break;
    case 5 : //get cmd
      dataIn.RX_CMD = data;
      debugIntState = 6;
      break;
    case 6 : //get FSRH
      dataIn.RX_FSRH = data;
      debugIntState = 7;
      break;
    case 7 : //get FSRL
      dataIn.RX_FSRL = data;
      debugIntState = 8;
      break;
    case 8 : //still processing
      break;
    default:
      debugIntState= 0;
      break;
  }
}

void Process_Debug_Stream(void)   /* Called from user_routines_fast.c */
{
  if (debugIntState == 8)  /* If data is ready to be processed */
  {
    dataOut.TX_INDEX = dataIn.RX_INDEX;  /* echo object index */
    dataOut.TX_CMD = dataIn.RX_CMD;      /* echo command */

    dataFromPC = 1;
    switch (dataIn.RX_CMD)
    {
      case READ_CMD:
        dataOut.TX_DATAH = 0;
        dataOut.TX_DATAL = 0xFF;
        if (dataIn.RX_INDEX == 0)
          dataOut.TX_DATAL = systemStateFlags;
        else if (dataIn.RX_INDEX == 1)
          dataOut.TX_DATAL = slavePtr->SystemFlags;
        else if (dataIn.RX_INDEX == 2)
          dataOut.TX_DATAL = TIM4->PSC;
        else if (dataIn.RX_INDEX == 3)
          dataOut.TX_DATAL = TIM4->ARR >> 8;
        else if (dataIn.RX_INDEX == 4)
          dataOut.TX_DATAL = slavePtr->MotorStatus[0];
        else if (dataIn.RX_INDEX == 5)
          dataOut.TX_DATAL = slavePtr->MotorStatus[1];
#if 0
        SendDataToUart ^= 1;
#else
        Debug_Send();
#endif
        break;
      case WRITE_CMD:
        //printf("index %d, dataL %0x\r\n",dataIn.RX_INDEX,dataIn.RX_DATAL);
        if (dataIn.RX_INDEX == 0)
          systemStateFlags = dataIn.RX_DATAL;        //Slave flags
        else if (dataIn.RX_INDEX == 1)
          slavePtr->SystemFlags = dataIn.RX_DATAL;
        else if (dataIn.RX_INDEX == 2)
          TIM4->PSC = dataIn.RX_DATAL;
        else if (dataIn.RX_INDEX == 3)
          TIM4->ARR = dataIn.RX_DATAL << 8 | 0xFF;
        else if (dataIn.RX_INDEX == 4)
          slavePtr->MotorStatus[0] = dataIn.RX_DATAL;
        else if (dataIn.RX_INDEX == 5)
          slavePtr->MotorStatus[1] = dataIn.RX_DATAL;
        break;
      case WRITE_PWMs:
        if (dataIn.RX_INDEX == 250) /* Special command to update PC_Control_Byte*/
          dataFromPC = dataIn.RX_DATAL;
        else
        {
          slavePtr->Motor[dataIn.RX_INDEX] = dataIn.RX_DATAL;
          slavePtr->MotorStatus[dataIn.RX_INDEX] = dataIn.RX_DATAL;
        }
        break;
    }
    debugIntState = 0;
  }
}

