#include "uart.h"

#include "uart.h"


#define string_size 80
char RX_FLAG_END_LINE = 0;
char RRX[string_size];
unsigned int RXI = 0;
char temp_char;
char a[10];

#define ENA_UART 1
void UART_Init(void)
{
  USART_InitTypeDef USART_InitStruct;
  GPIO_InitTypeDef GPIO_InitStruct;
  // Enable clock for USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
	
#if ENA_UART
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Initialization of USART1

  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART1, &USART_InitStruct);

  // Enable USART1
  USART_Cmd(USART1, ENABLE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  NVIC_EnableIRQ(USART1_IRQn);
#endif
}


void Send_char(char data)
{
	USART_SendData(USART1, data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Send_String(char *data)
{
	while(*data)
	{
		Send_char(*data);
		data++;
	}
}

void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		temp_char = USART_ReceiveData(USART1);
		
		if(temp_char != '\n')
		{	
			RRX[RXI] = temp_char;
			RXI++;
		}
		else
		{
			RRX[RXI] = 0x00;
			RX_FLAG_END_LINE=1;
			RXI = 0;
		}
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
