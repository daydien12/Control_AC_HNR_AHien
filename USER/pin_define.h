#ifndef _PIN_DF_
#define _PIN_DF_
	
#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"

#define OUT_ADC_1 		GPIO_Pin_0
#define OUT_ADC_2 		GPIO_Pin_1
#define OUT_ADC_3			GPIO_Pin_2
#define	LM35					GPIO_Pin_3
#define OUT_ADC_1_2_3_LM35_PORT		GPIOA

#define SPINSS				GPIO_Pin_4
#define SPISCK				GPIO_Pin_5
#define SPIMISO				GPIO_Pin_6
#define SPIMOSI				GPIO_Pin_7
#define SPI_CM_PORT								GPIOA

#define PWM1					GPIO_Pin_8
#define PWM2					GPIO_Pin_9
#define PWM1_PWM2_PORT						GPIOA	

#define CT_M1_Plus		GPIO_Pin_10
#define CT_M1_Minus		GPIO_Pin_11
#define CT_M1_PORT								GPIOA	

#define CT_BEEP				GPIO_Pin_12
#define CT_BEEP_PORT							GPIOA

#define CT_RL					GPIO_Pin_15
#define CT_RL_PORT								GPIOA


#define ADC_M1			GPIO_Pin_0
#define ADC_M2			GPIO_Pin_1		
#define ADC_M1_M2_PORT						GPIOB

#define CT_M2_Plus	GPIO_Pin_4
#define CT_M2_Minus	GPIO_Pin_5
#define CT_M2_PORT								GPIOB

#define BT_CT1			GPIO_Pin_7
#define BT_CT2			GPIO_Pin_6
#define BT_CT1_CT2_PORT						GPIOB


#ifdef __cplusplus
}
#endif

#endif
