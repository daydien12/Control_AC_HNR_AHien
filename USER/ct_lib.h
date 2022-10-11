#ifndef _CT_LIB_
#define _CT_LIB_
	
#ifdef __cplusplus
extern "C"{
#endif
#include "stm32f0xx_usart.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "Delay.h"

#define state_bt 0
enum BEEP
{
	BEEP_ON,
	BEEP_OFF,
	BEEP_BEEP,
};

enum RELAY
{
	RELAY_ON,
	RELAY_OFF,
};

typedef struct
{
	uint8_t Flag;
	uint8_t Hold;
	uint16_t count;
}_button_state_t;

extern volatile _button_state_t BT1, BT2;
void CT_Init(void);
void CT_BeeP(const uint8_t _mode_beep_, const uint16_t _time_);
void CT_Relay(const uint8_t _mode_relay_);
void CT_ReadAllButton(void);

#ifdef __cplusplus
}
#endif

#endif
