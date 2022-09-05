#include "motor_lib.h"
const uint16_t PWM_VALUE = 8000;
static uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
static void CT_Motor_PWM1(const uint16_t _duty_var_);
static void CT_Motor_PWM2(const uint16_t _duty_var_);
static void PWM_Init(void);
static void GPIO_Control_Init(void);

void CT_Motor_Init(void)
{
	PWM_Init();
	GPIO_Control_Init();
}

void CT_Motor_PWM1(const uint16_t _duty_var_)
{
	TIM1->CCR1 = map(_duty_var_,0,1000,0,PWM_VALUE);
}

void CT_Motor_PWM2(const uint16_t _duty_var_)
{
	TIM1->CCR2 = map(_duty_var_,0,1000,0,PWM_VALUE);
}

void CT_Motor_1(const uint16_t _pwm_var_, const uint8_t _direction_var_)
{
	CT_Motor_PWM1(_pwm_var_);
	if(MOTOR_RIGHT == _direction_var_)
	{
		GPIO_WriteBit(CT_M1_PORT, CT_M1_Plus, Bit_SET);
		GPIO_WriteBit(CT_M1_PORT, CT_M1_Minus, Bit_RESET);
	}
	else  if(MOTOR_LEFT == _direction_var_)
	{
		GPIO_WriteBit(CT_M1_PORT, CT_M1_Plus, Bit_RESET);
		GPIO_WriteBit(CT_M1_PORT, CT_M1_Minus, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(CT_M1_PORT, CT_M1_Plus, Bit_RESET);
		GPIO_WriteBit(CT_M1_PORT, CT_M1_Minus, Bit_RESET);
	}
}

void CT_Motor_2(const uint16_t _pwm_var_, const uint8_t _direction_var_)
{
	CT_Motor_PWM2(_pwm_var_);
	if(MOTOR_RIGHT == _direction_var_)
	{
		GPIO_WriteBit(CT_M2_PORT, CT_M2_Plus, Bit_SET);
		GPIO_WriteBit(CT_M2_PORT, CT_M2_Minus, Bit_RESET);
	}
	else if(MOTOR_LEFT == _direction_var_)
	{
		GPIO_WriteBit(CT_M2_PORT, CT_M2_Plus, Bit_RESET);
		GPIO_WriteBit(CT_M2_PORT, CT_M2_Minus, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(CT_M1_PORT, CT_M2_Plus, Bit_RESET);
		GPIO_WriteBit(CT_M1_PORT, CT_M2_Minus, Bit_RESET);
	}
}

static void PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	
	/* GPIOA Configuration: Channel 1, 2 */
  GPIO_InitStructure.GPIO_Pin = PWM1 | PWM2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(PWM1_PWM2_PORT, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);
	
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = PWM_VALUE;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

static void GPIO_Control_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = CT_M1_Plus | CT_M1_Minus;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(CT_M1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CT_M2_Plus | CT_M2_Minus;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(CT_M2_PORT, &GPIO_InitStructure);
}

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
