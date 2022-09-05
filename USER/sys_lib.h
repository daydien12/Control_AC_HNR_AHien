#ifndef _SYS_LIB_
#define _SYS_LIB_
	
#ifdef __cplusplus
extern "C"{
#endif
#include "stm32f0xx.h"
#include "Delay.h"

#include "pin_define.h"
#include "uart.h"
#include "ct_lib.h"
#include "motor_lib.h"
#include "spi_lib.h"
#include "adc_lib.h"
	
typedef struct
{
	uint16_t count_time_1s;
	uint8_t  flag_time_1s;
}_time_t;
	
void SYS_Init_ALL(void);
void SYS_Run(void);	
#ifdef __cplusplus
}
#endif

#endif
