#include p18f8520.inc

	radix   dec

; configuration registers
	__config        _CONFIG1H, 0xfe
	__config        _CONFIG2L, 0xfe
	__config        _CONFIG2H, 0xfc
	__config        _CONFIG3H, 0xfe
	__config        _CONFIG4L, 0xfb
	__config        _CONFIG5L, 0xff
	__config        _CONFIG5H, 0xff
	__config        _CONFIG6L, 0xff
	__config        _CONFIG6H, 0xff
	__config        _CONFIG7L, 0xff
	__config        _CONFIG7H, 0xff
;
;
;
COPY_VAR        udata   0x80

	global  gTX_BUFF0,gTX_BUFF1,gRX_BUFF0,gRX_BUFF1

gTX_BUFF0       res     32
gTX_BUFF1       res     32
gRX_BUFF0       res     32
gRX_BUFF1       res     32
;
;
;
SPI_VAR         udata_acs

	global  gSPI_RCV_PTR,gSPI_XMT_PTR,PWMdisableMask

gTX_PTRH        res     1
gTX_PTRL        res     1
gRX_PTRH        res     1
gRX_PTRL        res     1
FSRH_temp       res     1
FSRL_temp       res     1
gSPICNT         res     1
gSPI_RCV_PTR    res     1
gSPI_XMT_PTR    res     1
tmp             res     1
gPWM_DATA1      res     1
gPWM_DATA2      res     1
gPWM_DATA3      res     1
gPWM_DATA4      res     1
gPWM_DATA5      res     1
gPWM_DATA6      res     1
gPWM_DATA7      res     1
gPWM_DATA8      res     1
PORT_TRASH      res     1
Cnt             res     1
Cnt1            res     1
txPWM_MASK      res     1
PWMdisableMask  res     1
;
;
;
UTIL_LIB        code

	extern  Wait4TXEmpty,statusflag

	global  Clear_Memory
	global  Hex_output
	global  UpdateLocalPWMDirection
	global  SendDataToMaster
	global  GetDataFromMaster
	global  Generate_Pwms

;*****************************************************************************
;*****************************************************************************
Clear_Memory
	movlw   0
	movwf   FSR0L
	movwf   FSR0H

Clear_Loop
	clrwdt
	call    Clear_Bank
	incf    FSR0H, f
	movlw   0x8
	subwf   FSR0H, w
	btfss   STATUS, Z
	goto    Clear_Loop
	return

Clear_Bank
	clrf    INDF0
	incfsz  FSR0L, f
	goto    Clear_Bank
	return

;*****************************************************************************
;*****************************************************************************
Hex_output
	movlw   0xff
	movff   PLUSW1, tmp
	swapf   tmp, w
	andlw   0x0f
	addlw   0x6
	btfsc   STATUS, DC
	addlw   0x7
	addlw   0x2a
	movwf   TXREG1
	call    Wait4TXEmpty

	movf    tmp, w
	andlw   0x0f
	addlw   0x6
	btfsc   STATUS, DC
	addlw   0x7
	addlw   0x2a
	movwf   TXREG1
	call    Wait4TXEmpty

	retlw   0

;*****************************************************************************
;*****************************************************************************
UpdateLocalPWMDirection
	movlw   0xff
	movff   PLUSW1, txPWM_MASK
	btfsc   txPWM_MASK, 0
	bsf     TRISE, TRISE7
	btfss   txPWM_MASK, 0
	bcf     TRISE, TRISE7
	btfsc   txPWM_MASK, 1
	bsf     TRISG, TRISG0
	btfss   txPWM_MASK, 1
	bcf     TRISG, TRISG0
	btfsc   txPWM_MASK, 2
	bsf     TRISG, TRISG3
	btfss   txPWM_MASK, 2
	bcf     TRISG, TRISG3
	btfsc   txPWM_MASK, 3
	bsf     TRISG, TRISG4
	btfss   txPWM_MASK, 3
	bcf     TRISG, TRISG4
	btfsc   txPWM_MASK, 4
	bsf     TRISE, TRISE0
	btfss   txPWM_MASK, 4
	bcf     TRISE, TRISE0
	btfsc   txPWM_MASK, 5
	bsf     TRISE, TRISE1
	btfss   txPWM_MASK, 5
	bcf     TRISE, TRISE1
	btfsc   txPWM_MASK, 6
	bsf     TRISE, TRISE2
	btfss   txPWM_MASK, 6
	bcf     TRISE, TRISE2
	btfsc   txPWM_MASK, 7
	bsf     TRISE, TRISE3
	btfss   txPWM_MASK, 7
	bcf     TRISE, TRISE3
	return

;*****************************************************************************
;*****************************************************************************
SendDataToMaster
	movff   FSR0H, FSRH_temp
	movff   FSR0L, FSRL_temp
	movlw   0xff
	movff   PLUSW1, FSR0H
	movlw   0xfe
	movff   PLUSW1, FSR0L
	btfss   statusflag, 3
	bra     MoveData4Buff0

	movff   POSTINC0, gTX_BUFF1+0
	movff   POSTINC0, gTX_BUFF1+1
	movff   POSTINC0, gTX_BUFF1+2
	movff   POSTINC0, gTX_BUFF1+3
	movff   POSTINC0, gTX_BUFF1+4
	movff   POSTINC0, gTX_BUFF1+5
	movff   POSTINC0, gTX_BUFF1+6
	movff   POSTINC0, gTX_BUFF1+7
	movff   POSTINC0, gTX_BUFF1+8
	movff   POSTINC0, gTX_BUFF1+9
	movff   POSTINC0, gTX_BUFF1+10
	movff   POSTINC0, gTX_BUFF1+11
	movff   POSTINC0, gTX_BUFF1+12
	movff   POSTINC0, gTX_BUFF1+13
	movff   POSTINC0, gTX_BUFF1+14
	movff   POSTINC0, gTX_BUFF1+15
	movff   POSTINC0, gTX_BUFF1+16
	movff   POSTINC0, gTX_BUFF1+17
	movff   POSTINC0, gTX_BUFF1+18
	movff   POSTINC0, gTX_BUFF1+19
	movff   POSTINC0, gTX_BUFF1+20
	movff   POSTINC0, gTX_BUFF1+21
	movff   POSTINC0, gTX_BUFF1+22
	movff   POSTINC0, gTX_BUFF1+23
	movff   POSTINC0, gTX_BUFF1+24
	movff   POSTINC0, gTX_BUFF1+25
	movff   POSTINC0, gTX_BUFF1+26
	movff   POSTINC0, gTX_BUFF1+27
	movff   POSTINC0, gTX_BUFF1+28
	movff   POSTINC0, gTX_BUFF1+29
	movff   POSTINC0, gTX_BUFF1+30
	movff   POSTINC0, gTX_BUFF1+31
	bra     TerminateSend

MoveData4Buff0
	movff   POSTINC0, gTX_BUFF0+0
	movff   POSTINC0, gTX_BUFF0+1
	movff   POSTINC0, gTX_BUFF0+2
	movff   POSTINC0, gTX_BUFF0+3
	movff   POSTINC0, gTX_BUFF0+4
	movff   POSTINC0, gTX_BUFF0+5
	movff   POSTINC0, gTX_BUFF0+6
	movff   POSTINC0, gTX_BUFF0+7
	movff   POSTINC0, gTX_BUFF0+8
	movff   POSTINC0, gTX_BUFF0+9
	movff   POSTINC0, gTX_BUFF0+10
	movff   POSTINC0, gTX_BUFF0+11
	movff   POSTINC0, gTX_BUFF0+12
	movff   POSTINC0, gTX_BUFF0+13
	movff   POSTINC0, gTX_BUFF0+14
	movff   POSTINC0, gTX_BUFF0+15
	movff   POSTINC0, gTX_BUFF0+16
	movff   POSTINC0, gTX_BUFF0+17
	movff   POSTINC0, gTX_BUFF0+18
	movff   POSTINC0, gTX_BUFF0+19
	movff   POSTINC0, gTX_BUFF0+20
	movff   POSTINC0, gTX_BUFF0+21
	movff   POSTINC0, gTX_BUFF0+22
	movff   POSTINC0, gTX_BUFF0+23
	movff   POSTINC0, gTX_BUFF0+24
	movff   POSTINC0, gTX_BUFF0+25
	movff   POSTINC0, gTX_BUFF0+26
	movff   POSTINC0, gTX_BUFF0+27
	movff   POSTINC0, gTX_BUFF0+28
	movff   POSTINC0, gTX_BUFF0+29
	movff   POSTINC0, gTX_BUFF0+30
	movff   POSTINC0, gTX_BUFF0+31

TerminateSend
	movff   FSRH_temp, FSR0H
	movff   FSRL_temp, FSR0L
	return

;*****************************************************************************
;*****************************************************************************
GetDataFromMaster
	btfss   statusflag, 0
	return

	movff   FSR0H, FSRH_temp
	movff   FSR0L, FSRL_temp
	movlw   0xff
	movff   PLUSW1, FSR0H
	movlw   0xfe
	movff   PLUSW1, FSR0L
	btfss   statusflag, 4
	bra     MoveDataFromBuff1
	movff   gRX_BUFF0+0, POSTINC0
	movff   gRX_BUFF0+1, POSTINC0
	movff   gRX_BUFF0+2, POSTINC0
	movff   gRX_BUFF0+3, POSTINC0
	movff   gRX_BUFF0+4, POSTINC0
	movff   gRX_BUFF0+5, POSTINC0
	movff   gRX_BUFF0+6, POSTINC0
	movff   gRX_BUFF0+7, POSTINC0
	movff   gRX_BUFF0+8, POSTINC0
	movff   gRX_BUFF0+9, POSTINC0
	movff   gRX_BUFF0+10, POSTINC0
	movff   gRX_BUFF0+11, POSTINC0
	movff   gRX_BUFF0+12, POSTINC0
	movff   gRX_BUFF0+13, POSTINC0
	movff   gRX_BUFF0+14, POSTINC0
	movff   gRX_BUFF0+15, POSTINC0
	movff   gRX_BUFF0+16, POSTINC0
	movff   gRX_BUFF0+17, POSTINC0
	movff   gRX_BUFF0+18, POSTINC0
	movff   gRX_BUFF0+19, POSTINC0
	movff   gRX_BUFF0+20, POSTINC0
	movff   gRX_BUFF0+21, POSTINC0
	movff   gRX_BUFF0+22, POSTINC0
	movff   gRX_BUFF0+23, POSTINC0
	movff   gRX_BUFF0+24, POSTINC0
	movff   gRX_BUFF0+25, POSTINC0
	movff   gRX_BUFF0+26, POSTINC0
	movff   gRX_BUFF0+27, POSTINC0
	movff   gRX_BUFF0+28, POSTINC0
	movff   gRX_BUFF0+29, POSTINC0
	movff   gRX_BUFF0+30, POSTINC0
	movff   gRX_BUFF0+31, POSTINC0
	bra     TerminateGet

MoveDataFromBuff1
	movff   gRX_BUFF1+0, POSTINC0
	movff   gRX_BUFF1+1, POSTINC0
	movff   gRX_BUFF1+2, POSTINC0
	movff   gRX_BUFF1+3, POSTINC0
	movff   gRX_BUFF1+4, POSTINC0
	movff   gRX_BUFF1+5, POSTINC0
	movff   gRX_BUFF1+6, POSTINC0
	movff   gRX_BUFF1+7, POSTINC0
	movff   gRX_BUFF1+8, POSTINC0
	movff   gRX_BUFF1+9, POSTINC0
	movff   gRX_BUFF1+10, POSTINC0
	movff   gRX_BUFF1+11, POSTINC0
	movff   gRX_BUFF1+12, POSTINC0
	movff   gRX_BUFF1+13, POSTINC0
	movff   gRX_BUFF1+14, POSTINC0
	movff   gRX_BUFF1+15, POSTINC0
	movff   gRX_BUFF1+16, POSTINC0
	movff   gRX_BUFF1+17, POSTINC0
	movff   gRX_BUFF1+18, POSTINC0
	movff   gRX_BUFF1+19, POSTINC0
	movff   gRX_BUFF1+20, POSTINC0
	movff   gRX_BUFF1+21, POSTINC0
	movff   gRX_BUFF1+22, POSTINC0
	movff   gRX_BUFF1+23, POSTINC0
	movff   gRX_BUFF1+24, POSTINC0
	movff   gRX_BUFF1+25, POSTINC0
	movff   gRX_BUFF1+26, POSTINC0
	movff   gRX_BUFF1+27, POSTINC0
	movff   gRX_BUFF1+28, POSTINC0
	movff   gRX_BUFF1+29, POSTINC0
	movff   gRX_BUFF1+30, POSTINC0
	movff   gRX_BUFF1+31, POSTINC0
TerminateGet
	movff   FSRH_temp, FSR0H
	movff   FSRL_temp, FSR0L
	return

;*****************************************************************************
;*****************************************************************************
Generate_Pwms
#ifdef USE_TIMER
	btfsc   INTCON,TMR0IF
	return
#endif
	movf    PWMdisableMask, w
	xorlw   0x0f
	btfsc   STATUS, Z
	return

	movlw   0xff
	movff   PLUSW1, gPWM_DATA1
	movlw   0xfe
	movff   PLUSW1, gPWM_DATA2
	movlw   0xfd
	movff   PLUSW1, gPWM_DATA3
	movlw   0xfc
	movff   PLUSW1, gPWM_DATA4

	incf    gPWM_DATA1, f
	btfss   PWMdisableMask, 0
	bsf     LATE, LATE7

	incf    gPWM_DATA2, f
	btfss   PWMdisableMask, 1
	bsf     LATG, LATG0

	incf    gPWM_DATA3, f
	btfss   PWMdisableMask, 2
	bsf     LATG, LATG3

	incf    gPWM_DATA4, f
	btfss   PWMdisableMask, 3
	bsf     LATG, LATG4

	movlw   0xfb
	movff   PLUSW1, gPWM_DATA5
	movlw   0xfa
	movff   PLUSW1, gPWM_DATA6
	movlw   0xf9
	movff   PLUSW1, gPWM_DATA7
	movlw   0xf8
	movff   PLUSW1, gPWM_DATA8

	incf    gPWM_DATA5, f
	bsf     LATE, LATE0

	incf    gPWM_DATA6, f
	bsf     LATE, LATE1

	incf    gPWM_DATA7, f
	bsf     LATE, LATE2

	incf    gPWM_DATA8, f
	bsf     LATE, LATE3

Generate_Pwms_Cont
	call    Dead_Space_Loop
LP4
	decfsz  gPWM_DATA1, f
	goto    LP4_Cont1
	btfss   PWMdisableMask, 0
	bcf     LATE, LATE7

LP4_Cont1
	decfsz  gPWM_DATA2, f
	goto    LP4_Cont2
	btfss   PWMdisableMask, 1
	bcf     LATG, LATG0

LP4_Cont2
	decfsz  gPWM_DATA3, f
	goto    LP4_Cont3
	btfss   PWMdisableMask, 2
	bcf     LATG, LATG3

LP4_Cont3
	nop
	decfsz  gPWM_DATA4, f
	goto    LP4_Cont4
	btfss   PWMdisableMask, 3
	bcf     LATG, LATG4

LP4_Cont4
	nop
	decfsz  gPWM_DATA5
	goto    LP4_Cont5
	bcf     LATE, LATE0

LP4_Cont5
	nop
	decfsz  gPWM_DATA6
	goto    LP4_Cont6
	bcf     LATE, LATE1

LP4_Cont6
	nop
	decfsz  gPWM_DATA7
	goto    LP4_Cont7
	bcf     LATE, LATE2

LP4_Cont7
	nop
	decfsz  gPWM_DATA8
	goto    LP4_Cont8
	bcf     LATE, LATE3

LP4_Cont8
	call    Adjust4
	incfsz  Cnt
	goto    LP4
	return

Dead_Space_Loop
	movlw   0xd7
	movwf   Cnt
DL1
	nop
	nop
	nop
	nop
	call    Pwm_Delay
	decfsz  Cnt
	goto    DL1

	movlw   0xd7
	movwf   Cnt
DL2
	nop
	nop
	nop
	nop
	call    Pwm_Delay
	decfsz  Cnt
	goto    DL2

	nop
	return

Pwm_Delay
	nop
	goto    s_delay
s_delay
	nop
	nop
	nop
	nop
	nop
	nop
	return

Adjust4
	movlw   3
	movwf   Cnt1
Adj_lp4
	nop
	decfsz  Cnt1
	goto    Adj_lp4
	return

	end
