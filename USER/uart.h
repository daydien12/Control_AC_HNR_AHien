#ifndef _UART_
#define _UART_
	
#ifdef __cplusplus
extern "C"{
#endif
#include "stm32f0xx_usart.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
	
extern char RRX[];	
extern char RX_FLAG_END_LINE;
	
void UART_Init(void);
void Send_char(char data);
void Send_String(char *data);
	
#ifdef __cplusplus
}
#endif

#endif
