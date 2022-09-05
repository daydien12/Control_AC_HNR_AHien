#ifndef _SPI_LIB_
#define _SPI_LIB_
	
#ifdef __cplusplus
extern "C"{
#endif
	
#include "stm32f0xx.h"
#include "pin_define.h"
#include "Delay.h"

void SPI_Command_Init(void);
uint16_t SPIx_Transfer(uint16_t data);
void SPIx_EnableSlave(void);
void SPIx_DisableSlave(void);
 
#ifdef __cplusplus
}
#endif

#endif
