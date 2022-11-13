#include "ct_lib.h"
#include "pin_define.h"

#define INPUT_BT1 GPIO_ReadInputDataBit(BT_CT1_CT2_PORT, BT_CT1)
#define INPUT_BT2 GPIO_ReadInputDataBit(BT_CT1_CT2_PORT, BT_CT2)

#define ENABLE_BEEP 1
volatile _button_state_t BT1, BT2;

static void ct_buttonread(const uint8_t _input_signal_,_button_state_t *_button_var_);

void CT_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  // Enable clock for USART1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
	/*================(Config BEEP)================*/
  GPIO_InitStruct.GPIO_Pin = CT_BEEP;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(CT_BEEP_PORT, &GPIO_InitStruct);
	
	/*================(Config Relay)================*/
  GPIO_InitStruct.GPIO_Pin = CT_RL;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(CT_RL_PORT, &GPIO_InitStruct);
	
	/*================(Config Button)================*/
  GPIO_InitStruct.GPIO_Pin = BT_CT1|BT_CT2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BT_CT1_CT2_PORT, &GPIO_InitStruct);
}

void CT_BeeP(const uint8_t _mode_beep_, const uint16_t _time_)
{
	if(ENABLE_BEEP == 1)
	{
		if(_mode_beep_ == BEEP_ON)
		{
			GPIO_WriteBit(CT_BEEP_PORT, CT_BEEP, Bit_SET);
		}
		else if(_mode_beep_ == BEEP_OFF)
		{
			GPIO_WriteBit(CT_BEEP_PORT, CT_BEEP, Bit_RESET);
		}
		else if(_mode_beep_ == BEEP_BEEP)
		{
			GPIO_WriteBit(CT_BEEP_PORT, CT_BEEP, Bit_SET);
			delay_ms(_time_/2);
			GPIO_WriteBit(CT_BEEP_PORT, CT_BEEP, Bit_RESET);
			delay_ms(_time_/2);
		}
	}
}

void CT_Relay(const uint8_t _mode_relay_)
{
	if(_mode_relay_ == RELAY_ON)
	{
		GPIO_WriteBit(CT_RL_PORT, CT_RL, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(CT_RL_PORT, CT_RL, Bit_RESET);
	}
}

void CT_ReadAllButton(void)
{
	ct_buttonread(INPUT_BT1, (_button_state_t*)&BT1);
	ct_buttonread(INPUT_BT2, (_button_state_t*)&BT2);
}

static void ct_buttonread(const uint8_t _input_signal_,_button_state_t *_button_var_)
{
	if(_input_signal_ == state_bt)
	{
		if(_button_var_->count <= 200)
		{
			_button_var_->count++;
			_button_var_->Hold = 0;
			if(_button_var_->count == 50)
			{
					_button_var_->Flag = 1;
			}
		}
		else
		{
			_button_var_->Hold = 1;
			_button_var_->count = 2100;
		}
	}
	else
	{
		_button_var_->count = 0;
		_button_var_->Hold = 0;
	}
}
