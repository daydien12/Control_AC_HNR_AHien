#ifndef _MT_LIB_
#define _MT_LIB_
	
#ifdef __cplusplus
extern "C"{
#endif
	
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "pin_define.h"
#include "Delay.h"

enum MT_direction
{
	MOTOR_RIGHT,
	MOTOR_LEFT,
	MOTOR_OFF,
};
	
void CT_Motor_Init(void);
void CT_Motor_1(const uint16_t _pwm_var_, const uint8_t _direction_var_);
void CT_Motor_2(const uint16_t _pwm_var_, const uint8_t _direction_var_);		
#ifdef __cplusplus
}
#endif

#endif
