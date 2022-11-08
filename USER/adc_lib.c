#include "adc_lib.h"

volatile uint8_t aa = 0, count_empty_time;
volatile _adc_value_t_ Adc_VarArr;
static uint32_t Arr_Channel[6] = {ADC_Channel_0, ADC_Channel_1, ADC_Channel_2, ADC_Channel_3, ADC_Channel_8, ADC_Channel_9};
static _adc_channel_e channel_read_all = ADC_Channel_0_OUT_ADC_1;
static void read_adc_all(uint16_t *_data_, const _adc_channel_e _adc_channel_, _adc_kalman_filter_t_ *_kamal_var);
_adc_kalman_filter_t_ Kalman_OUT_ADC_1, Kalman_OUT_ADC_2, Kalman_OUT_ADC_3, Kalman_LM35, Kalman_ADC_M1, Kalman_ADC_M2;

void ADC_Init_All(void)
{
  ADC_InitTypeDef   ADC_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  GPIO_InitStructure.GPIO_Pin = OUT_ADC_1 | OUT_ADC_2 | OUT_ADC_3 | LM35;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(OUT_ADC_1_2_3_LM35_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_M1 | ADC_M2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(ADC_M1_M2_PORT, &GPIO_InitStructure);

  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_GetCalibrationFactor(ADC1);

  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

  NVIC_EnableIRQ(ADC1_COMP_IRQn);

  ADC_Cmd(ADC1, ENABLE);

  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
	Kalman_OUT_ADC_1.init_data = 0;
}

static void read_adc_all(uint16_t *_data_, const _adc_channel_e _adc_channel_, _adc_kalman_filter_t_  *_kamal_var)
{
	if(count_empty_time>=20)
	{
		if (Adc_VarArr.adc_config == 0)
		{
			ADC1->CHSELR = (uint32_t)Arr_Channel[channel_read_all];
			Adc_VarArr.adc_count = 0;
			Adc_VarArr.adc_flag = 0;
			Adc_VarArr.adc_config = 1;
			ADC_StartOfConversion(ADC1);
			Adc_VarArr.adc_value = 0;
		}
		else
		{
			if (Adc_VarArr.adc_flag == 1)
			{
				count_empty_time = 0;
				Adc_VarArr.adc_config = 0;
				*_data_ = ADC_Kalman_Filter(Adc_VarArr.adc_value, _kamal_var);
				//*_data_ = Adc_VarArr.adc_value;
				channel_read_all = _adc_channel_;
			}
		}
	}
	else
	{
		count_empty_time++;
	}
}

uint16_t ADC_Read_One_Channel(const _adc_channel_e ADC_Channel, const uint32_t ADC_Sample)
{
  uint32_t tmpreg = 0;
  ADC1->CHSELR = (uint32_t)Arr_Channel[ADC_Channel];
  tmpreg &= ~ADC_SMPR1_SMPR;
  tmpreg |= (uint32_t)ADC_Sample;
  ADC1->SMPR = tmpreg;

  Adc_VarArr.adc_count = 0;
  Adc_VarArr.adc_flag = 0;
  ADC_StartOfConversion(ADC1);
  while (!Adc_VarArr.adc_flag);
  return Adc_VarArr.adc_value;
}

void ADC_Read_All(uint16_t *_adc_arr_data_)
{
  ADC1->SMPR = ADC_SampleTime_239_5Cycles;
	switch (channel_read_all)
	{
		case ADC_Channel_0_OUT_ADC_1:
			read_adc_all(&_adc_arr_data_[0], ADC_Channel_1_OUT_ADC_2, &Kalman_OUT_ADC_1);
			
			break;

		case ADC_Channel_1_OUT_ADC_2:
			read_adc_all(&_adc_arr_data_[1], ADC_Channel_2_OUT_ADC_3, &Kalman_OUT_ADC_2);
			break;

		case ADC_Channel_2_OUT_ADC_3:
			read_adc_all(&_adc_arr_data_[2], ADC_Channel_3_ADC_LM35, &Kalman_OUT_ADC_3);
			break;

		case ADC_Channel_3_ADC_LM35:
			read_adc_all(&_adc_arr_data_[3], ADC_Channel_8_ADC_M1, &Kalman_LM35);
			break;

		case ADC_Channel_8_ADC_M1:
			read_adc_all(&_adc_arr_data_[4], ADC_Channel_9_ADC_M2, &Kalman_ADC_M1);
			break;

		case ADC_Channel_9_ADC_M2:
			read_adc_all(&_adc_arr_data_[5], ADC_Channel_0_OUT_ADC_1, &Kalman_ADC_M2);
			Adc_VarArr.adc_flag_ReadALL = 1;
			break;
	}
}

uint16_t ADC_Kalman_Filter(unsigned long ADC_Value, _adc_kalman_filter_t_ *_adc_kalman_)
{
		float x_k1_k1,x_k_k1;
		uint16_t kalman_adc;
	  float Z_k;
		if(_adc_kalman_->init_data == 0)
		{
			_adc_kalman_->ADC_OLD_Value = 0.0;
			_adc_kalman_->P_k1_k1 = 0.0;
			_adc_kalman_->kalman_adc_old=0;
			_adc_kalman_->Q = 0.0001; //Q: Regulation noise, Q increases, dynamic response becomes faster, and convergence stability becomes worse
			_adc_kalman_->R = 0.0005;    //R: Test noise, R increases, dynamic response becomes slower, convergence stability becomes better
			_adc_kalman_->Kg = 0;
			_adc_kalman_->P_k_k1 = 1;
			_adc_kalman_->init_data = 1;
		}
		
    Z_k = ADC_Value;
    x_k1_k1 = _adc_kalman_->kalman_adc_old;

    x_k_k1 = x_k1_k1;
    _adc_kalman_->P_k_k1 = _adc_kalman_->P_k1_k1 + _adc_kalman_->Q;

    _adc_kalman_->Kg = _adc_kalman_->P_k_k1/(_adc_kalman_->P_k_k1 + _adc_kalman_->R);

    kalman_adc = x_k_k1 + _adc_kalman_->Kg * (Z_k - _adc_kalman_->kalman_adc_old);
    _adc_kalman_->P_k1_k1 = (1 - _adc_kalman_->Kg)*_adc_kalman_->P_k_k1;
    _adc_kalman_->P_k_k1 = _adc_kalman_->P_k1_k1;

    _adc_kalman_->ADC_OLD_Value = ADC_Value;
    _adc_kalman_->kalman_adc_old = kalman_adc;
	return kalman_adc;
}

void ADC1_IRQHandler()
{
	uint16_t tempadc = 0;
  if (ADC_GetITStatus(ADC1, ADC_IT_EOC))
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    if (Adc_VarArr.adc_count >= NUMBER_READ_ADC)
    {
      Adc_VarArr.adc_flag = 1;
      ADC_StopOfConversion(ADC1);
    }
    else
    {
			tempadc = ADC_GetConversionValue(ADC1);
			if(tempadc > Adc_VarArr.adc_value)
			{
				Adc_VarArr.adc_value = tempadc;
			}
      ADC_DiscModeCmd(ADC1, ENABLE);
      ADC_StartOfConversion(ADC1);
      Adc_VarArr.adc_count ++; 
		}
	}
}
