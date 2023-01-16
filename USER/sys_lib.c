#include "sys_lib.h"
#include "stdio.h"
#include "IWDG.h" 
#define ENA_TEST_FREQUENCY 1
uint8_t sys_test_pwm = 0;
//MOTOR_LEFT ve 0
#define linear_equations(a,x,b) ((a*x)+(b))
uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#define MODE_TEST_MOTOR_TRAI_PHAI 0
#define MODE_TEST_MOTOR_BUTTON_TRAI_PHAI 1

static uint16_t	VOUT_AVG = 220;
static _time_t Time_tick;
static void SYS_TIM3_Init(void);
static uint16_t Adc_Arr_Convert[6];
static _sys_value_t sys_var;

static void sys_assign_adc(void);

static void sys_status_init(void);
static void sys_status_stanby(void);
static void sys_status_normal_loading(void);
static void sys_status_normal_overvoltage(void);
static void sys_status_normal_undervoltage(void);
static void sys_status_normal_overload(void);
static void sys_status_abnormal_overvoltage(void);
static void sys_status_abnormal_undervoltage(void);
static void sys_status_abnormal_overload(void);
static void sys_status_reset(void);
static void sys_status_error(void);

static void sys_control_motor(void);
static void sys_Test_motor(uint8_t mode);

static float Table_Correction_volt_in[3][2] = 
{ 
{0.2, -9.00},
{0.1818, 10.72},
{0.2011, -11.80758}
};

static float Table_Correction_volt_out[3][2] = 
{
{0.2, -9.60},
{0.1898, 0.2531},
{0.20720, -21.54954}
};

void SYS_Init_ALL(void)
{
	#if (ENA_TEST_FREQUENCY == 1)
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPIMISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(SPI_CM_PORT, &GPIO_InitStructure);
	#endif
	
  UART_Init();
  ADC_Init_All();
  CT_Init();
  //CT_Motor_Init();
  SYS_TIM3_Init();
	#if (ENA_TEST_FREQUENCY == 0)
  SPI_Command_Init();
	#endif
	sys_var.flag_stanby = 0;
	sys_var.volt_out2  = 7;
	sys_var.flag_init = 0;
	Time_tick.count_time_init_5s = 0;
	IWD_Init(5);
	sys_status_reset();
	Send_String("reset\n");
}

void SYS_Run(void)
{
	char arr[100];
	spi_struct_type_t datatest;
//	
//	if(sys_var.flag_init == 0)
//	{
//		sys_status_init();
//		CT_Motor_1(1000,MOTOR_LEFT);
//		if(Time_tick.count_time_init_5s >= 4)
//		{
//			sys_var.flag_init = 1;
//			sys_status_stanby();
//			//sys_var.volt_out2 = 7;
//		}
//		else
//		{
//			Time_tick.count_time_init_5s++;
//		}
//		delay_ms(1000);
//	}
//	else
//	{
//		if (BT1.Flag == 1)
//		{
//			BT1.Flag = 0;
//			if(VOUT_AVG < VOUT_MAX)
//			{
//				VOUT_AVG++;
//			}
//		}

//		if (BT2.Flag == 1)
//		{
//			BT2.Flag = 0;
//			if(VOUT_AVG > VOUT_MIN)
//			{
//				VOUT_AVG--;
//			}
//		}
//		
//		if((VIN_MIN < sys_var.volt_in)&&(sys_var.volt_in < VIN_MAX))
//		{
//			if(!sys_var.flag_stanby)
//			{
//				
//				if(sys_var.volt_out2 <= 1)
//				{
//					sys_var.flag_stanby = 1;
//					sys_var.volt_out2 = 0;
//				}
//				else
//				{
//					sys_var.volt_out2--;
//				}
//				delay_ms(1000);
//			}
//			else
//			{
//				if((VOUT_MIN < sys_var.volt_out1)&&(sys_var.volt_out1 < VOUT_MAX)&&(sys_var.amp_in < AMP_MAX))
//				{
//					sys_status_normal_loading();
//				}
//				else if((VOUT_MAX <= sys_var.volt_out1)&&(sys_var.volt_out1 <= VOUT_NOR_OVERVOL_MAX)&&(sys_var.amp_in < AMP_MAX))
//				{
//					sys_status_normal_overvoltage();
//				}
//				else if((VOUT_NOR_UNDERVOL_MIN <= sys_var.volt_out1)&&(sys_var.volt_out1 <= VOUT_MIN)&&(sys_var.amp_in < AMP_MAX))
//				{
//					sys_status_normal_undervoltage();
//				}
//				else if((VOUT_MIN <= sys_var.volt_out1)&&(sys_var.volt_out1 <= VOUT_MAX)&&(AMP_MAX <= sys_var.amp_in)&&(sys_var.amp_in < AMP_ABNOR_MAX))
//				{
//					sys_status_normal_overload();
//				}
//				else if((sys_var.volt_out1 > VOUT_NOR_OVERVOL_MAX))
//				{
//					sys_status_abnormal_overvoltage();
//				}
//				else if((sys_var.volt_out1 < VOUT_NOR_UNDERVOL_MIN))
//				{
//					sys_status_abnormal_undervoltage();
//				}
//				else if((sys_var.amp_in > AMP_ABNOR_MAX))
//				{
//					sys_status_abnormal_overload();
//				}
//				else 
//				{
//					sys_var.Status_sig = STT_STANBY;
//				}
//			}
//		}
//		else
//		{
//			sys_status_error();
//		}
//	}
	
	if(Time_tick.flag_time_1s)
	{
		Time_tick.flag_time_1s = 0;
		sys_assign_adc();
		Adc_VarArr.adc_flag_ReadALL = 0;
		
		//sprintf(arr, "adc0:%d adc1:%d adc2:%f adc3:%d adc8:%d adc9:%d\n",  sys_var.volt_in, sys_var.volt_out1, sys_var.amp_in, sys_var.value_temp, sys_var.volt_13m1, sys_var.volt_13m2);
		//sprintf(arr, "Variable 1: %f | %f\n", sys_var.amp_in*100, (float)((float)((sys_var.amp_in*sys_var.amp_in)*2.2) - (float)(sys_var.amp_in*30.7) + (106)));
		sprintf(arr, "adc0:%d adc1:%d adc2:%d adc3:%d adc8:%d adc9:%d\n", Adc_Arr_Convert[0], Adc_Arr_Convert[1], Adc_Arr_Convert[2], Adc_Arr_Convert[3], Adc_Arr_Convert[4], Adc_Arr_Convert[5]);
		//Send_String(arr);
		//sprintf(arr, "adc0:%d adc1:%d volt_in:%d volt_out1:%d PC817:%d\n", Adc_Arr_Convert[0], Adc_Arr_Convert[1], sys_var.volt_in, sys_var.volt_out1, sys_var.frequency);
		//sprintf(arr, "adc0:%d adc1:%d in:%d out1:%d  out2:%d init:%d\n", Adc_Arr_Convert[0], Adc_Arr_Convert[1], sys_var.volt_in, sys_var.volt_out1, sys_var.volt_out2, Time_tick.count_time_init_5s);
		Send_String(arr);
		#if (ENA_TEST_FREQUENCY == 0)
		datatest.status_sig 	= sys_var.Status_sig; 							//1 byte
		datatest. volt_in 		= sys_var.volt_in;									//2 byte
		datatest. amp_in 			= (uint16_t)(sys_var.amp_in*100);		//4 byte
		datatest. volt_out1 	= sys_var.volt_out1;								//2 byte
		datatest. volt_out2 	= sys_var.volt_out2;								//2 byte
		datatest. value_temp 	= 10;  															//1 byte
		datatest. value_freq 	= sys_var.frequency;								//1 byte
		Transfer_Data(&datatest);
		#endif
	}
}



static void sys_assign_adc(void)
{
	if (Adc_VarArr.adc_flag_ReadALL == 1)
	{
		Adc_VarArr.adc_flag_ReadALL = 0;
		sys_var.flag_adc_control_mottor = 1;

		/*
		sys_var.volt_in				= Adc_Arr_Convert[0];
		sys_var.volt_out1 		= Adc_Arr_Convert[1];
		sys_var.amp_in 				= Adc_Arr_Convert[2];
		sys_var.value_temp 		= Adc_Arr_Convert[3];
		sys_var.volt_13m1 		= Adc_Arr_Convert[4];
		sys_var.volt_13m2 		= Adc_Arr_Convert[5];
		*/
		
		if(Adc_Arr_Convert[0] <= 1041)
		{
			sys_var.volt_in	= linear_equations(Adc_Arr_Convert[0], Table_Correction_volt_in[0][0], Table_Correction_volt_in[0][1]);
		}
		else if((Adc_Arr_Convert[0] > 1041) && (Adc_Arr_Convert[0]<=1206))
		{
			sys_var.volt_in	= linear_equations(Adc_Arr_Convert[0], Table_Correction_volt_in[1][0], Table_Correction_volt_in[1][1]);
		}
		else if(Adc_Arr_Convert[0] > 1206)
		{
			sys_var.volt_in	= linear_equations(Adc_Arr_Convert[0], Table_Correction_volt_in[2][0], Table_Correction_volt_in[2][1]);
		}
	
		if(Adc_Arr_Convert[1] <= 1052)
		{
			sys_var.volt_out1	= linear_equations(Adc_Arr_Convert[1], Table_Correction_volt_out[0][0], Table_Correction_volt_out[0][1]);
		}
		else if((Adc_Arr_Convert[1] > 1052) && (Adc_Arr_Convert[1]<=1210))
		{
			sys_var.volt_out1	= linear_equations(Adc_Arr_Convert[1], Table_Correction_volt_out[1][0], Table_Correction_volt_out[1][1]);
		}	
		else if(Adc_Arr_Convert[1] > 1210)
		{
			sys_var.volt_out1	= linear_equations(Adc_Arr_Convert[1], Table_Correction_volt_out[2][0], Table_Correction_volt_out[2][1]);
		}
		
		sys_var.amp_in 				= Adc_Arr_Convert[2]/100.0;
		sys_var.value_temp 		= Adc_Arr_Convert[3];
		sys_var.volt_13m1 		= Adc_Arr_Convert[4];
		sys_var.volt_13m2 		= Adc_Arr_Convert[5];
	}
}

static void SYS_TIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = 10-1;
  TIM_TimeBaseStructure.TIM_Prescaler = 4800-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_Cmd(TIM3, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM3_IRQn);
}

/*time 1ms*/
void TIM3_IRQHandler(void) 
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		IWD_Clear_Time_Out();
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if (Time_tick.count_time_1s >= 1000)
    {
      Time_tick.flag_time_1s = 1;
			Time_tick.count_time_1s = 0;
    }
    else
    {
      Time_tick.count_time_1s++;
    }
		
		if(sys_var.flag_init)
		{
			sys_control_motor();
		}
		
    CT_ReadAllButton();
		ADC_Read_All(Adc_Arr_Convert);
		
//		if(INPUT_PC817 == 1)
//		{
//			Time_tick.count_frequency++;
//		}
//		else
//		{
//			if(Time_tick.count_frequency > 10)
//			{
//				sys_var.frequency = (1.0/(float)Time_tick.count_frequency)*1000;
//				if(sys_var.frequency > 48 && sys_var.frequency < 58)
//				{
//					sys_var.frequency = 50;
//				}
//				else if(sys_var.frequency >= 58 && sys_var.frequency < 68)
//				{
//					sys_var.frequency = 60;
//				}
//			}
//			Time_tick.count_frequency = 0;
//		}
  }
}

static void sys_status_init(void)
{
	sys_var.Status_sig = STT_INIT;
	CT_Relay(RELAY_ON);
	CT_BeeP(BEEP_OFF, 0);
	sys_var.flag_normal_loading = 0;
}

static void sys_status_stanby(void)
{
		sys_var.Status_sig = STT_STANBY;
		CT_Relay(RELAY_ON);
		CT_BeeP(BEEP_OFF, 0);
		sys_var.flag_normal_loading = 0;
}

static void sys_status_normal_loading(void)
{
		sys_var.Status_sig = STT_NORMAL_LOADING;
		CT_Relay(RELAY_OFF);
		if(sys_var.flag_normal_loading == 0)
		{
			CT_BeeP(BEEP_ON, 0);
			delay_ms(500);
			CT_BeeP(BEEP_OFF, 0);
			sys_var.flag_normal_loading = 1;
		}
		sys_var.volt_out2 = sys_var.volt_out1;
}

static void sys_status_normal_overvoltage(void)
{
		sys_var.Status_sig = STT_NORMAL_OVERVOLTAGE;
		CT_Relay(RELAY_OFF);
		CT_BeeP(BEEP_BEEP, TIME_BEEP);
		sys_var.volt_out2 = sys_var.volt_out1;
		sys_var.flag_normal_loading = 0;
}

static void sys_status_normal_undervoltage(void)
{
		sys_var.Status_sig = STT_NORMAL_UNDERVOLTAGE;
		CT_Relay(RELAY_OFF);
		CT_BeeP(BEEP_BEEP, TIME_BEEP);
		sys_var.volt_out2 = sys_var.volt_out1;
		sys_var.flag_normal_loading = 0;
}

static void sys_status_normal_overload(void)
{
		sys_var.Status_sig = STT_NORMAL_OVERLOAD;
		CT_Relay(RELAY_OFF);
		CT_BeeP(BEEP_BEEP, TIME_BEEP);
		sys_var.volt_out2 = sys_var.volt_out1;
		sys_var.flag_normal_loading = 0;
}

static void sys_status_abnormal_overvoltage(void)
{
		sys_var.Status_sig = STT_ABNORMAL_OVERVOLTAGE;
		CT_Relay(RELAY_ON);
		CT_BeeP(BEEP_BEEP, TIME_BEEP);
		sys_var.volt_out2 = 0;
		sys_var.flag_normal_loading = 0;
}

static void sys_status_abnormal_undervoltage(void)
{
		sys_var.Status_sig = STT_ABNORMAL_UNDERVOLTAGE;
		CT_Relay(RELAY_ON);
		CT_BeeP(BEEP_BEEP, TIME_BEEP);
		sys_var.volt_out2 = 0;
		sys_var.flag_normal_loading = 0;
}

static void sys_status_abnormal_overload(void)
{
		sys_var.Status_sig = STT_ABNORMAL_OVERLOAD;
		CT_Relay(RELAY_ON);
		CT_BeeP(BEEP_BEEP, TIME_BEEP);
		sys_var.volt_out2 = 0;
		sys_var.flag_normal_loading = 0;
}

static void sys_status_reset(void)
{
		sys_var.flag_normal_loading = 0;
}

static void sys_status_error(void)
{
	sys_var.Status_sig = STT_ERROR;
	CT_Relay(RELAY_ON);
	CT_BeeP(BEEP_ON, 0);
	sys_var.flag_normal_loading = 0;
}

static void sys_control_motor(void)
{
	uint16_t Speed_mottor, caculater_speed = 0;
	if(sys_var.flag_adc_control_mottor)
	{
		sys_var.flag_adc_control_mottor = 0;
		if((sys_var.volt_in >= 150) && (sys_var.volt_out1 >= 150))
		{
			if(sys_var.volt_out1 > 225)
			{
				caculater_speed = (sys_var.volt_out1 - 220);
				Speed_mottor = map(caculater_speed, 0, 80, 510,1000);
				CT_Motor_1(Speed_mottor,MOTOR_LEFT);
			}
			else if(sys_var.volt_out1 < 218)
			{
				caculater_speed = (220-sys_var.volt_out1);
				Speed_mottor = map(caculater_speed, 0, 80, 520,1000);
				CT_Motor_1(Speed_mottor,MOTOR_RIGHT);
			}
			else if((sys_var.volt_out1 > 218) && (sys_var.volt_out1 < 223)) 
			{
				CT_Motor_1(1000,MOTOR_OFF);
			}
		}
		else
		{
			CT_Motor_1(1000,MOTOR_OFF);
		}
	}
}


static void sys_Test_motor(uint8_t mode)
{
	if(mode == 1)
	{
		if (BT1.Hold == 1)
		{
			CT_Motor_1(1000,MOTOR_LEFT);
		}
		else if (BT2.Hold == 1)
		{
			CT_Motor_1(1000,MOTOR_RIGHT);
		}
		else
		{
			CT_Motor_1(1000,MOTOR_OFF);
		}
	}
	else
	{
		CT_Motor_1(1000,MOTOR_LEFT);
		delay_ms(4500);
		CT_Motor_1(1000,MOTOR_RIGHT);
		delay_ms(4500);
	}
}
void SYS_Test_Pwm(void)
{
	sys_test_pwm =! sys_test_pwm;
	GPIO_WriteBit(SPI_CM_PORT, SPIMISO, sys_test_pwm);
}