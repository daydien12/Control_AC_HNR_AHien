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

#define	VIN_MIN									30
#define VIN_MAX									300
#define VOUT_MIN								200
#define VOUT_MAX								235
#define VOUT_NOR_UNDERVOL_MIN		170
#define VOUT_NOR_OVERVOL_MAX		250
#define AMP_MAX									85
#define	AMP_ABNOR_MAX						100
#define VOLT13_AVG							13

#define TIME_BEEP 500

typedef enum
{
	STT_INIT = 0,
	STT_STANBY,
	STT_NORMAL_LOADING,
	STT_NORMAL_OVERVOLTAGE,
	STT_NORMAL_UNDERVOLTAGE,
	STT_NORMAL_OVERLOAD,
	STT_ABNORMAL_OVERVOLTAGE,
	STT_ABNORMAL_UNDERVOLTAGE,
	STT_ABNORMAL_OVERLOAD,
	STT_RESET,
	STT_ERROR,
}_sys_status_e;

typedef struct
{
	uint16_t count_time_1s;
	uint8_t  flag_time_1s;
	uint16_t count_time_20ms;
	uint8_t  flag_time_20ms;
	uint8_t  flag_time_count_stanby;
	uint8_t  flag_time_count_init;
	uint8_t  flag_time_count_handle;
	uint8_t  flag_start_count_handle;
	uint16_t count_time_init_5s;
	uint16_t count_time_handle_2s;
}_time_t;

typedef struct
{
	_sys_status_e Status_sig;
	uint16_t volt_in;
	float amp_in;
	uint16_t volt_out1;
	uint16_t volt_out2;
	uint16_t value_temp;
	uint16_t value_freq;
	uint16_t volt_13m1;
	uint16_t volt_13m2;
	uint8_t  flag_stanby;
	uint8_t flag_normal_loading;
	uint8_t flag_init;
	uint8_t flag_adc_control_mottor;
}_sys_value_t;



void SYS_Init_ALL(void);
void SYS_Run(void);	
#ifdef __cplusplus
}
#endif

#endif
