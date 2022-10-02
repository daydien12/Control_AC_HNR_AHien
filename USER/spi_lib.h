#ifndef _SPI_LIB_
#define _SPI_LIB_
	
#ifdef __cplusplus
extern "C"{
#endif
	
#include "stm32f0xx.h"
#include "pin_define.h"
#include "Delay.h"

typedef struct 
{
	uint8_t status_sig; 	//1 byte
	uint16_t volt_in;			//2 byte
	uint16_t amp_in;      //2 byte
	uint16_t volt_out1;		//2 byte
	uint16_t volt_out2;		//2 byte
	uint8_t value_temp;   //1 byte
	uint8_t value_freq;		//1 byte
	//uint8_t	byte_zero;		//1 byte
}spi_struct_type_t;

	
void SPI_Command_Init(void);
uint16_t SPIx_Transfer(uint16_t data);
void SPIx_EnableSlave(void);
void SPIx_DisableSlave(void);
void Transfer_Data(spi_struct_type_t *data);
#ifdef __cplusplus
}
#endif

#endif
