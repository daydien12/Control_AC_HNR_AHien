#include "spi_lib.h"

void SPI_Command_Init(void)
{
	// Initialization struct
    SPI_InitTypeDef SPI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
 
    // Step 1: Initialize SPI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	  //SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct); 
    SPI_Cmd(SPI1, ENABLE);
 
    // Step 2: Initialize GPIO
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
		GPIO_PinAFConfig(SPI_CM_PORT, GPIO_PinSource4, GPIO_AF_0);	
		GPIO_PinAFConfig(SPI_CM_PORT, GPIO_PinSource5, GPIO_AF_0);
		GPIO_PinAFConfig(SPI_CM_PORT, GPIO_PinSource6, GPIO_AF_0);
		GPIO_PinAFConfig(SPI_CM_PORT, GPIO_PinSource7, GPIO_AF_0);
		
    // GPIO pins for MOSI, MISO, and SCK
    GPIO_InitStructure.GPIO_Pin = SPISCK | SPIMISO | SPIMOSI;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(SPI_CM_PORT, &GPIO_InitStructure);
    // GPIO pin for SS
    GPIO_InitStructure.GPIO_Pin = SPINSS;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_Init(SPI_CM_PORT, &GPIO_InitStructure);
		
    //// Disable SPI slave device
    SPIx_DisableSlave();
}
uint16_t SPIx_Transfer(uint16_t data)
{
  while (!(SPI1->SR & SPI_SR_TXE));      				// убедиться, что предыдущая передача завершена (STM32F103)
	SPI1->DR=data;										// вывод в SPI1
	while (!(SPI1->SR & SPI_SR_RXNE));     				// ждем окончания обмена (STM32F103)
	return SPI1->DR;		         					// читаем принятые данные
}

void Transfer_Data(spi_struct_type_t *data)
{
	uint16_t *tempdata = (uint16_t*)data;
	uint8_t i = 0;
	SPIx_EnableSlave();
	for(i=0; i<8; i++)
	{
		SPIx_Transfer(tempdata[i]);
	}
	
	SPIx_DisableSlave();
}


void SPIx_EnableSlave()
{
    // Set slave SS pin low
    SPI_CM_PORT->BRR = SPINSS;
}
 
void SPIx_DisableSlave()
{
    // Set slave SS pin high
    SPI_CM_PORT->BSRR = SPINSS;
}