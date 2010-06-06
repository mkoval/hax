#include <spi.h>

#include "ifi/default.h"
#include "ifi/aliases.h"

/*******************************************************************************
			   FUNCTION PROTOTYPES
*******************************************************************************/

// external asm function prototypes
extern void UpdateLocalPWMDirection(unsigned char pwm_mask);
extern void GetDataFromMaster(rx_data_ptr ptr);
extern void SendDataToMaster(tx_data_ptr ptr);

// local function prototypes
void InterruptHandlerHigh(void);
void Setup_Spi_Slave(void);
void Initialize_Registers(void);
void IFI_Initialization(void);
void User_Proc_Is_Ready(void);
void Prep_SPI_4_First_Byte(void);
void Handle_Spi_Int(void);
void Getdata(rx_data_ptr ptr);
void Putdata(tx_data_ptr ptr);
void Clear_SPIdata_flag(void);
void Setup_PWM_Output_Type(int pwmSpec1,int pwmSpec2,int pwmSpec3,int pwmSpec4);
void Check_4_Violations(tx_data_ptr ptr);

/*******************************************************************************
			    VARIABLE DECLARATIONS
*******************************************************************************/

extern unsigned char gRX_BUFF0[];
extern unsigned char gRX_BUFF1[];
extern unsigned char gTX_BUFF0[];
extern unsigned char gTX_BUFF1[];
extern unsigned char PWMdisableMask;

unsigned char *xferbufr;                // not used?
unsigned char *txferbufr;       // probably should be volatile
unsigned char *rxferbufr;       // probably should be volatile
unsigned char I;                                // not used?
unsigned char count;            // probably should be volatile
unsigned char temp;                     // probably should be volatile
unsigned char *rxPtr;                   // not used?
unsigned char *txPtr;                   // not used?
unsigned char OneShot;
unsigned char AutoOn;

#if defined(MCC18)

#pragma code InterruptVectorHigh=0x808
void InterruptVectorHigh(void)
{
	_asm 
		GOTO isr_high 
	_endasm
}
#pragma code

#elif defined(SDCC)

void ivt_high(void) __naked __interrupt 1
{
	__asm
		goto _isr_high
	__endasm;
}

#endif /* defined(SDCC) */

#if defined(MCC18_30)
#pragma interrupt isr_high nosave=TBLPTR, TABLAT, PCLATH, PROD, section(".tmpdata"), section("MATH_DATA")
#elif defined(MCC18_24)
#pragma interrupt isr_high
#endif
void isr_high(void)
{
	if ( INTCONbits.INT0IF )
		Prep_SPI_4_First_Byte();
	else if ( PIR1bits.SSPIF )
		Handle_Spi_Int();
}

//#pragma code

/*******************************************************************************
* FUNCTION NAME: Setup_Spi_Slave
* PURPOSE:       
* CALLED FROM:   this file (Initialize_Registers)
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Setup_Spi_Slave(void)
{
	OpenSPI(SLV_SSOFF, MODE_01, SMPMID);
}

/*******************************************************************************
* FUNCTION NAME: Initialize_Registers
* PURPOSE:       
* CALLED FROM:   this file (IFI_Initialization)
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Initialize_Registers(void)
{
	CMCON = 7;
	PORTA = PORTB = PORTC = PORTD = PORTE = PORTF = PORTG = PORTH = PORTJ = 0;  
	LATA = LATB = LATC = LATD = LATE = LATF = LATG = LATH = LATJ = 0;  
	TRISA = 0xff;
	TRISB = 0xff;
	TRISC = 0xbf;
	TRISD = 0;
	TRISE = 0x70;
	TRISF = 0x9f;
	TRISG = 0xe4;
	TRISH = 0x0f;
	TRISJ = 0xff;
	ADCON1 = 0x0f;
	ADCON2 = 0x07;
	INTCON2bits.NOT_RBPU = 0;
	PSPCONbits.PSPMODE = 0;
	MEMCONbits.EBDIS = 1;
	IPR1 = IPR2 = 0;
	PIE1 = PIE2 = 0;
	IPR3 = PIE3 = 0;

	Setup_Spi_Slave();

	IPR1bits.SSPIP = 1;
	RCONbits.IPEN = 1;
	PIR1bits.SSPIF = 0;
	PIE1bits.SSPIE = 1;
	TRISBbits.TRISB1 = 1;
	INTCON3 = 0;
	INTCONbits.INT0IE = 1;
	INTCON3bits.INT2IP = 0;
	INTCON3bits.INT2IE = 0;
	INTCON2bits.INT3IP = 0;
	INTCON3bits.INT3IE = 0;
	INTCON2 = 0;
	INTCONbits.GIEL = 1;
	INTCONbits.GIEH = 1;
}

/*******************************************************************************
* FUNCTION NAME: IFI_Initialization
* PURPOSE:       Configure registers and initializes the SPI RX/TX buffers.
* CALLED FROM:   main.c
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void IFI_Initialization(void)
{
	unsigned char i;

	Initialize_Registers();

	statusflag.TX_BUFFSELECT = 0;
	statusflag.RX_BUFFSELECT = 0;

	txPtr = &txdata.rc_pwm01;
	for ( i=0; i<16; i++ )
		*(txPtr+i) = 0x7f;

	txdata.packetnum = 0;
	txdata.current_mode  = 1;
	txdata.control = 0xc9;
	statusflag.FIRST_TIME = 1;
#ifdef USE_TIMER
	T0CON = 0x01;
	TMR0H = 0x80;
	TMR0L = 0xf9;
	T0CONbits.TMR0ON = 0;
#endif
#ifdef COMPETITION
	if (PORTBbits.RB7 == 0)
	{
		txdata.user_cmd = 0x18;
		txdata.spare01 = 0x2d;
		txdata.spare02 = 0;
	}
	else
	{
		txdata.user_cmd = 0x10;
		txdata.spare02 = 0x78;
	}
#endif
	Putdata(&txdata);
	statusflag.TX_UPDATED = 0;
	Putdata(&txdata);
	statusflag.TX_UPDATED = 0;
}

/*******************************************************************************
* FUNCTION NAME: User_Proc_Is_Ready
* PURPOSE:       This routine informs the master processor that all user
*                initalization has been complete.  The call to this routine must
*                always be the last line of User_Initialization.
* CALLED FROM:   user_routines.c
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void User_Proc_Is_Ready(void)
{
	PORTCbits.RC1 = 1;
	PORTAbits.RA4 = 1;
	TRISCbits.TRISC1 = 0;
	TRISAbits.TRISA4 = 0;
}

/*******************************************************************************
* FUNCTION NAME: Prep_SPI_4_First_Byte
* PURPOSE:       
* CALLED FROM:   this file (InterruptHandlerHigh)
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Prep_SPI_4_First_Byte(void)
{
	INTCONbits.INT0IF = 0;
	count = 0x20;
	IPR1bits.SSPIP = 1;

	if ( statusflag.RX_BUFFSELECT )
		rxferbufr = gRX_BUFF1;
	else
		rxferbufr = gRX_BUFF0;

	if ( !statusflag.NEW_SPI_DATA )
		statusflag.RX_BUFFSELECT = !statusflag.RX_BUFFSELECT;

	if ( statusflag.TX_BUFFSELECT )
		txferbufr = gTX_BUFF0;
	else
		txferbufr = gTX_BUFF1;

	temp = SSPBUF;
	SSPBUF = *txferbufr++;
}

/*******************************************************************************
* FUNCTION NAME: Handle_Spi_Int
* PURPOSE:       
* CALLED FROM:   this file (InterruptHandlerHigh)
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Handle_Spi_Int(void)
{
	PIR1bits.SSPIF = 0;
	temp = SSPBUF;

	if ( count > 0 )
	{
		*rxferbufr++ = temp;

		temp = *txferbufr++;    
		SSPBUF = temp;

		count--;
		if ( count == 0 )
		{
			txdata.current_mode = 2;
			statusflag.TX_UPDATED = 0;
			statusflag.NEW_SPI_DATA = 1;
#ifdef USE_TIMER
			TMR0H = 0x77;
			TMR0L = 0x47;
			INTCONbits.TMR0IF = 0;
			T0CONbits.TMR0ON = 1;
#endif
		}
	}
	else
		SSPBUF = 0;
}

/*******************************************************************************
* FUNCTION NAME: Getdata
* PURPOSE:       Retrieve data from the SPI receive buffer sent by the master
*                microprocessor.  This routine takes 14.8 us to complete.
* CALLED FROM:   user_routines(_fast).c
* ARGUMENTS:
*     Argument       Type           IO   Description
*     --------       -----------    --   -----------
*          ptr       rx_data_ptr    I    pointer to the receive buffer
* RETURNS:       void
*******************************************************************************/
void Getdata(rx_data_ptr ptr)
{
	if ( statusflag.NEW_SPI_DATA )
	{
		GetDataFromMaster(ptr);

		PIE1bits.SSPIE = 0;
		statusflag.NEW_SPI_DATA = 0;
		PIE1bits.SSPIE = 1;

		if ( PWMdisableMask != 0x0f )
			UpdateLocalPWMDirection(txdata.pwm_mask);

#ifdef AUTONOMOUS
		if (rxdata.rc_receiver_status_byte.allbits == 1)
		{
			if ( (int)rxdata.oi_analog05 > 0x9a && !OneShot )
			{
				AutoOn ^= 1;
				OneShot = 1;
			}
			else if ( (int)rxdata.oi_analog05 < 0x9a )
				OneShot = 0;

			// XXX: What? (see below as well)
			rxdata.rc_mode_byte.mode.autonomous = AutoOn;
		}

		if ( rxdata.rc_receiver_status_byte.allbits == 0 )
			AutoOn = 0;
#endif
#ifdef COMPETITION
		AutoOn = (int)(!PORTBbits.RB7);

		if ( rxdata.rc_receiver_status_byte.allbits == 0 )
			AutoOn = 0;

		// XXX: Why the hell are we setting things in the recieved data?
		rxdata.rc_mode_byte.mode.autonomous = AutoOn;
#endif
#if defined(COMPETITION) || defined(AUTONOMOUS)
		if ( AutoOn )
		{
			// XXX: Again. 
			rxdata.rc_receiver_status_byte.allbits = 4;
			rxdata.oi_analog01 = rxdata.oi_analog02 = rxdata.oi_analog03 = rxdata.oi_analog04 = 0x7f;
			rxdata.oi_analog05 = rxdata.oi_analog06 = rxdata.oi_analog07 = rxdata.oi_analog08 = 0x7f;
			rxdata.oi_analog09 = rxdata.oi_analog10 = rxdata.oi_analog11 = rxdata.oi_analog12 = 0x7f;
		}
#endif
	}
}

/*******************************************************************************
* FUNCTION NAME: Putdata
* PURPOSE:       Fill the transmit buffer with data to send out to the master
*                microprocessor. This routine takes 23 us to complete.
* CALLED FROM:   user_routines(_fast).c
* ARGUMENTS:
*     Argument       Type           IO   Description
*     --------       -----------    --   -----------
*          ptr       tx_data_ptr    I    pointer to the transmit buffer
* RETURNS:       void
*******************************************************************************/
void Putdata(tx_data_ptr ptr)
{
	if ( !statusflag.TX_UPDATED )
	{
		if ( ptr->current_mode == 2 )
			Check_4_Violations(ptr);

		//ptr->cmd_byte1 &= 0b01111111; // clear the highest bit of cmd_byte1
		ptr->cmd_byte1 &= ~(1<<7);
		SendDataToMaster(ptr);
		PIE1bits.SSPIE = 0;
		statusflag.TX_BUFFSELECT = !statusflag.TX_BUFFSELECT;
		statusflag.TX_UPDATED = 1;
		PIE1bits.SSPIE = 1;
		txdata.packetnum++;
	}
}

/*******************************************************************************
* FUNCTION NAME: Clear_SPIdata_flag
* PURPOSE:       
* CALLED FROM:   not used?
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Clear_SPIdata_flag(void)
{
	PIE1bits.SSPIE = 0;
	statusflag.NEW_SPI_DATA = 0;
	PIE1bits.SSPIE = 1;
}

/*******************************************************************************
* FUNCTION NAME: Setup_PWM_Output_Type
* PURPOSE:
* CALLED FROM:   user_routines.c
* ARGUMENTS:
*     Argument       Type           IO   Description
*     --------       -----------    --   -----------
*     pwmSpec1       int            I    pwm13 type definition (IFI_PWM or USER_CCP)
*     pwmSpec2       int            I    pwm14 type definition (IFI_PWM or USER_CCP)
*     pwmSpec3       int            I    pwm15 type definition (IFI_PWM or USER_CCP)
*     pwmSpec4       int            I    pwm16 type definition (IFI_PWM or USER_CCP)
* RETURNS:       void
*******************************************************************************/
void Setup_PWM_Output_Type(int pwmSpec1,int pwmSpec2,int pwmSpec3,int pwmSpec4)
{
	PWMdisableMask = 0;

	if ( pwmSpec1 == USER_CCP )
		PWMdisableMask |= 1<<0;
	else
		CCP2CON = 0;

	if ( pwmSpec2 == USER_CCP )
		PWMdisableMask |= 1<<1;
	else
		CCP3CON = 0;
		
	if ( pwmSpec3 == USER_CCP )
		PWMdisableMask |= 1<<2;
	else
		CCP4CON = 0;

	if ( pwmSpec4 == USER_CCP )
		PWMdisableMask |= 1<<3;
	else
		CCP5CON = 0;
}

/*******************************************************************************
* FUNCTION NAME: Check_4_Violations
* PURPOSE:       
* CALLED FROM:   this file (Putdata)
* ARGUMENTS:
*     Argument       Type           IO   Description
*     --------       -----------    --   -----------
*          ptr       tx_data_ptr    I    pointer to the transmit buffer
* RETURNS:       void
*******************************************************************************/
void Check_4_Violations(tx_data_ptr ptr)
{
	ptr->error_code = 0;
	ptr->warning_code = 0;

	if ( SSPCON1bits.WCOL ) 
	// spi write collision
	{
		SSPCON1bits.WCOL = 0;
		ptr->warning_code = 1;
	}
	else if ( (INTCON2bits.NOT_RBPU == 1) && (INTCONbits.RBIE == 1) )
	// PORTB Pullups disabled or RB Port Chaange interrupt enable
	{
		ptr->warning_code = 2;
	}
	else if ( INTCON3bits.INT1IE )
	// External interrupt 1 enabled
	{
		LATHbits.LATH7 = 0; // PortH.pin7 = low
		ptr->warning_code = 3;
	}

	if ( (SSPCON1 & 0x25) != 0x25 )
	// 0x25 = 0b00100101
	// What they check: !(SSPEN == 1 && SSPM2 == 1 && SSPM0 == 1)
	// What they should check:
	// !(SSPEN == 1 && SSPM3 == 0 && SSPM2 == 1 && SSMP1 == 0 && SSPM0 == 1)
	{
		ptr->error_code = 1;
		ptr->warning_code = SSPCON1;
	}
	else if ( SSPCON2 )
	// Using I2C mode instead of spi.
	{
		ptr->error_code = 2;
		ptr->warning_code = SSPCON2;
	}
	else if ( !RCONbits.IPEN )
	// Interrupt priority disabled.
	{
		ptr->error_code = 3;
		ptr->warning_code = RCON;
	}
	else if ( !PIE1bits.SSPIE )
	// SPI Interrupt dissabled.
	{
		ptr->error_code = 4;
		ptr->warning_code = PIE1;
	}
	else if ( MEMCON )
	// Page 73. External memory.
	{
		ptr->error_code = 5;
		ptr->warning_code = MEMCON;
	}
	else if ( IPR1 != 0x08 )
	// 0x08 = 0b00001000, bit3
	// Some interupt in IPR1 other than SSPIP is high priority.
	{
		ptr->error_code = 6;
		ptr->warning_code = IPR1;
	}
	else if ( IPR2 )
	// some interrupt in IPR2 is high priority
	{
		ptr->error_code = 7;
		ptr->warning_code = IPR2;
	}
	else if ( IPR3 )
	// some interrupt in IPR3 is high priority
	{
		ptr->error_code = 8;
		ptr->warning_code = IPR3;
	}
	else if ( (INTCON & 0x10) != 0x10 )
	// 0x10 = 0b00010000, bit4
	// if Exteral Int0 is disabled.
	{
		ptr->error_code = 9;
		ptr->warning_code = INTCON;
	}
	else if ( INTCON2 & 0b01000111 )
	// bits{0,1,2,6} == 1
	//x0:  RBIP: RB Port Change Interrupt Priority bit
	//x1:  INT3IP: INT3 External Interrupt Priority bit
	//x2:  TMR0IP: TMR0 Overflow Interrupt Priority bit
	// 3:  INTEDG3: External Interrupt 3 Edge Select bit
	// 4:  INTEDG2: External Interrupt 2 Edge Select bit
	// 5:  INTEDG1: External Interrupt 1 Edge Select bit
	//x6:  INTEDG0: External Interrupt 0 Edge Select bit
	// 7:  RBPU: PORTB Pull-up Enable bit
	//
	// if (PortB pin change isr high proroity || INT3 high priority \
	//     || TMR0 Ovf High Priority || INT0 edge is on rising edge )
	// if ( some INT is high prority || INT0 edge is on rising edge ) 
	{
		ptr->error_code = 10;
		ptr->warning_code = INTCON2;
	}
	else if ( INTCON3 & 0b11000000 )
	//x7: INT2 Priority
	//x6: INT1 Priority
	// if ( INT2 or IN1 is high priority )
	{
		ptr->error_code = 11;
		ptr->warning_code = INTCON3;
	}
	else if ( TRISAbits.TRISA4 )
	// If PortA pin4 is not an output:
	{
		ptr->error_code = 12;
		ptr->warning_code = TRISA;
	}
	else if ( !TRISBbits.TRISB0 )
	// If PortB pin0 is not an input :
	{
		ptr->error_code = 13;
		ptr->warning_code = TRISB;
	}
	else if ( (TRISC & 0x3e) != 0x1c )
	// 0x3e = 0b00111110
	// 0x1c = 0b00011100
	// 1@0:
	// 2@1:
	// 3@1:
	// 4@1:
	// 5@0:
	// PortC: 
	//		pin1 and pin5 are not outputs
	//	or	pin2, pin3, and pin4 are not inputs
	{
		ptr->error_code = 14;
		ptr->warning_code = TRISC;
	}
	else if ( !TRISFbits.TRISF7 )
	// if (PortF pin7 is not an input)
	{
		ptr->error_code = 15;
		ptr->warning_code = TRISF;
	}
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
