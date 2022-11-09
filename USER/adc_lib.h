#ifndef _ADC_LIB_
#define _ADC_LIB_
	
#ifdef __cplusplus
extern "C"{
#endif
#include "stm32f0xx_usart.h"
#include "stm32f0xx.h"
#include "Delay.h"
#include "pin_define.h"


	
typedef struct
{
	uint16_t 	adc_count;
	uint8_t 	adc_flag;
	uint8_t 	adc_config;
	uint8_t		adc_flag_ReadALL;
	uint16_t	adc_value;

}_adc_value_t_;
extern volatile _adc_value_t_ Adc_VarArr;	

typedef enum
{
	ADC_Channel_0_OUT_ADC_1 = 0	,
	ADC_Channel_1_OUT_ADC_2			,
	ADC_Channel_2_OUT_ADC_3			,
	ADC_Channel_3_ADC_LM35			,
	ADC_Channel_8_ADC_M1				,
	ADC_Channel_9_ADC_M2				,
}_adc_channel_e;

typedef struct
{
	float ADC_OLD_Value;
	float P_k1_k1;

	float Q;
	float R; 
	float Kg;
	float P_k_k1;
	float kalman_adc_old;
	
	uint8_t init_data;
}_adc_kalman_filter_t_;
extern _adc_kalman_filter_t_ Kalman_OUT_ADC_1, Kalman_OUT_ADC_2, Kalman_OUT_ADC_3, Kalman_LM35, Kalman_ADC_M1, Kalman_ADC_M2;


//#define NUMBER_READ_ADC 500 

//float Table_Correction[2][2];//= {{0.198, -7.0297}, {0.198, -7.0297}};
 
void ADC_Init_All(void);
void ADC_Read_All(uint16_t *_adc_arr_data_);	
uint16_t ADC_Read_One_Channel(const _adc_channel_e ADC_Channel, const uint32_t ADC_Sample);
uint16_t ADC_Kalman_Filter(unsigned long ADC_Value, _adc_kalman_filter_t_ *_adc_kalman_);
#ifdef __cplusplus
}
#endif

#endif
