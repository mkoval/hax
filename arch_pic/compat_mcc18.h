#ifndef COMPAT_MCC18_H_
#define COMPAT_MCC18_H_

#ifdef MCC18

#define delay1ktcy Delay1KTCYx
#define delay10tcy Delay10TCYx

#define adc_conv ConvertADC
#define adc_busy BusyADC
#define adc_read ReadADC
#define adc_setchannel SetChanADC

#define usart1_open Open1USART
#define usart1_busy Busy1USART
#define usart1_putc Write1USART

#endif /* def MCC18 */

#endif /* COMPAT_MCC18_H_ */
