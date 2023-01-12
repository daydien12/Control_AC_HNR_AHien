

#include "stm32f0xx.h"
#include "Delay.h"
#include "pin_define.h"
uint16_t delay;
uint8_t stt1 = 0;

static uint8_t  fac_us=0;
static uint16_t fac_ms=0; 


void SysTick_Handler (void)
{
	if (delay > 0)delay--;
//		stt1 =! stt1;
//	GPIO_WriteBit(SPI_CM_PORT, SPIMISO, stt1);
}

void delay_ms (uint16_t del)
{
	SysTick_Config (SystemCoreClock/1000);
	delay = del;
	while (delay);
}

void delay_us (uint16_t del)
{
	SysTick_Config (SystemCoreClock/1000000);
	delay = del;
	while (delay);
}


//==============================delay===============================
void Fn_DELAY_Init (unsigned char _CLK)
{
	SysTick->CTRL&=0xfffffffb;        
	fac_us=_CLK/8;		    
	fac_ms=(uint16_t)fac_us*1000;
}



void Fn_DELAY_ms (unsigned int _vrui_Time)
{	 		  	  		              //72M??????,nms<=1864
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)_vrui_Time*fac_ms;    //??????(SysTick->LOAD?24bit)
	SysTick->VAL =0x00;               //????????
	SysTick->CTRL=0x01 ;              //??'????  
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&(!(temp&(1<<16))));//????????
	SysTick->CTRL=0x00;               //????????
	SysTick->VAL =0X00;               //????????	  	    
}  


void Fn_DELAY_us (unsigned long _vrui_Time)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=_vrui_Time*fac_us;         //??????	  		 
	SysTick->VAL=0x00;                //????????
	SysTick->CTRL=0x01 ;              //??'???? 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&(!(temp&(1<<16))));//????????
	SysTick->CTRL=0x00;               //????????
	SysTick->VAL =0X00;               //????????	 
}
