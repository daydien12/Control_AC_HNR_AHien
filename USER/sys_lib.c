#include "sys_lib.h"
#include "stdio.h"
//MOTOR_LEFT ve 0
#define linear_equations(a,x,b) ((a*x)+(b))

#define MODE_TEST_MOTOR_TRAI_PHAI 0
#define MODE_TEST_MOTOR_BUTTON_TRAI_PHAI 1
volatile int abb = 0;
static uint16_t	VOUT_AVG = 220;
static _time_t Time_tick;
static void SYS_TIM3_Init(void);
static uint16_t Adc_Arr_Convert[6];
static _sys_value_t sys_var;

static void sys_assign_adc(void);

static void sys_status_init(void);
static uint8_t sys_status_stanby(void);
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
static void sys_spi_transmission(void);
static void sys_Test_motor(uint8_t mode);
static float Table_Correction_volt_in[2][2] = {
{0.198, -7.0297}, 
{0.2, -9.00}
};
void SYS_Init_ALL(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPISCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(SPI_CM_PORT, &GPIO_InitStructure);
	
	
  UART_Init();
  ADC_Init_All();
  CT_Init();
  CT_Motor_Init();
  SYS_TIM3_Init();
  SPI_Command_Init();
	sys_status_init();
	sys_var.flag_stanby = 0;
	sys_var.volt_out2  = 7;
	CT_Motor_1(1000,MOTOR_LEFT);
	delay_ms(4500);
}

void SYS_Run(void)
{
	char arr[100];
	spi_struct_type_t datatest;

  if (BT1.Flag == 1)
  {
    BT1.Flag = 0;
		if(VOUT_AVG < VOUT_MAX)
		{
			VOUT_AVG++;
		}
  }

  if (BT2.Flag == 1)
  {
    BT2.Flag = 0;
		if(VOUT_AVG > VOUT_MIN)
		{
			VOUT_AVG--;
		}
  }
	sys_Test_motor(MODE_TEST_MOTOR_BUTTON_TRAI_PHAI);

	if((VIN_MIN < sys_var.volt_in)&&(sys_var.volt_in < VIN_MAX))
	{
		if(!sys_var.flag_stanby)
		{
			if(	sys_status_stanby() == 1)
			{
				sys_var.flag_stanby = 1;
			}
		}
		else
		{
			if((VOUT_MIN < sys_var.volt_out1)&&(sys_var.volt_out1 < VOUT_MAX)&&(sys_var.amp_in < AMP_MAX))
			{
				sys_status_normal_loading();
			}
			else if((VOUT_MAX <= sys_var.volt_out1)&&(sys_var.volt_out1 <= VOUT_NOR_OVERVOL_MAX)&&(sys_var.amp_in < AMP_MAX))
			{
				sys_status_normal_overvoltage();
			}
			else if((VOUT_NOR_UNDERVOL_MIN <= sys_var.volt_out1)&&(sys_var.volt_out1 <= VOUT_MIN)&&(sys_var.amp_in < AMP_MAX))
			{
				sys_status_normal_undervoltage();
			}
			else if((VOUT_MIN <= sys_var.volt_out1)&&(sys_var.volt_out1 <= VOUT_MAX)&&(AMP_MAX <= sys_var.amp_in)&&(sys_var.amp_in < AMP_ABNOR_MAX))
			{
				sys_status_normal_overload();
			}
			else if((sys_var.volt_out1 > VOUT_NOR_OVERVOL_MAX))
			{
				sys_status_abnormal_overvoltage();
			}
			else if((sys_var.volt_out1 < VOUT_NOR_UNDERVOL_MIN))
			{
				sys_status_abnormal_undervoltage();
			}
			else if((sys_var.amp_in > AMP_ABNOR_MAX))
			{
				sys_status_abnormal_overload();
			}
			else 
			{
				sys_var.Status_sig = STT_STANBY;
			}
		}
	}
	else
	{
		sys_status_error();
	}

	//sys_control_motor();
	if(Time_tick.flag_time_1s)
	{
		Time_tick.flag_time_1s = 0;
		sys_assign_adc();
		Adc_VarArr.adc_flag_ReadALL = 0;
		sprintf(arr, "adc0:%d adc1:%d adc2:%f adc3:%d adc8:%d adc9:%d\n",  sys_var.volt_in, sys_var.volt_out1, sys_var.amp_in, sys_var.value_temp, sys_var.volt_13m1, sys_var.volt_13m2);
		//sprintf(arr, "Variable 1: %d\n", sizeof(spi_struct_type_t));
		//sprintf(arr, "Variable 1: %d - %f\n", sys_var.amp_in, (((float)sys_var.amp_in*0.00738)+ (-3.7343)));
		//sprintf(arr, "Variable 1: %d \n", sys_var.volt_in);
		//sprintf(arr, "spi_struct_type_t:%d \n", sizeof(spi_struct_type_t));
		//Send_String(arr);
		//((sys_var.volt_in*0.8)+138.407)
		//Time_tick.flag_time_1s = 0;
		//sprintf(arr, "adc0:%d \n", VOUT_AVG);
		//Send_String(arr);	

		datatest.status_sig 	= sys_var.Status_sig; 	//1 byte
		datatest. volt_in 		= sys_var.volt_in;	//2 byte
		datatest. amp_in 			=  (uint16_t)(sys_var.amp_in*100);	//4 byte
		datatest. volt_out1 	= sys_var.volt_out1;	//2 byte
		datatest. volt_out2 	= sys_var.volt_out2;	//2 byte
		datatest. value_temp 	= 10;  //1 byte
		datatest. value_freq 	= 50;	//1 byte
		Transfer_Data(&datatest);
	}
	
}

static void sys_assign_adc(void)
{
	if (Adc_VarArr.adc_flag_ReadALL == 1)
	{
		Adc_VarArr.adc_flag_ReadALL = 0;
		//sprintf(arr, "adc0:%0.2f adc1:%d adc2:%d adc3:%d adc8:%d adc9:%d\n", (float)((Adc_Arr_Convert[0]*1.083)+136.2499), Adc_Arr_Convert[1], Adc_Arr_Convert[2], Adc_Arr_Convert[3], Adc_Arr_Convert[4], Adc_Arr_Convert[5]);
		//sprintf(arr, "adc0:%d adc1:%d adc2:%d adc3:%d adc8:%d adc9:%d\n", Adc_Arr_Convert[0], Adc_Arr_Convert[1], Adc_Arr_Convert[2], Adc_Arr_Convert[3], Adc_Arr_Convert[4], Adc_Arr_Convert[5]);
		//Send_String(arr);
		/*
		sys_var.volt_in				= Adc_Arr_Convert[0];
		sys_var.volt_out1 		= Adc_Arr_Convert[1];
		sys_var.amp_in 				= Adc_Arr_Convert[2];
		sys_var.value_temp 		= Adc_Arr_Convert[3];
		sys_var.volt_13m1 		= Adc_Arr_Convert[4];
		sys_var.volt_13m2 		= Adc_Arr_Convert[5];
		*/
		//*
		sys_var.volt_in 			= linear_equations(Adc_Arr_Convert[0], Table_Correction_volt_in[0][0], Table_Correction_volt_in[0][1]);
		/*
		if(sys_var.volt_in <= 30)
		{
			sys_var.volt_in = 30;
		}
		else if(sys_var.volt_in >= 300)
		{
				sys_var.volt_in = 300;
		}
		*/
		sys_var.volt_out1 			= linear_equations(Adc_Arr_Convert[1], Table_Correction_volt_in[0][0], Table_Correction_volt_in[0][1]);
		/*
		if(sys_var.volt_out1 <= 30)
		{
			sys_var.volt_out1 = 30;
		}
		else if(sys_var.volt_out1 >= 300)
		{
				sys_var.volt_out1 = 300;
		}
		*/
		
		//sys_var.volt_out1 		= Adc_Arr_Convert[1];
		sys_var.amp_in 				= (((float)Adc_Arr_Convert[2]*0.00738)+ (-3.7343));;
		sys_var.value_temp 		= Adc_Arr_Convert[3];
		sys_var.volt_13m1 		= Adc_Arr_Convert[4];
		sys_var.volt_13m2 		= Adc_Arr_Convert[5];
		//*/
	}
}

static void SYS_TIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = 7995; //47955
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_Cmd(TIM3, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		//GPIO_WriteBit(GPIOA, SPISCK, abb);
		//abb =! abb;
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if (Time_tick.count_time_1s >= 1000)
    {
      Time_tick.count_time_1s = 0;
      Time_tick.flag_time_1s = 1;
			Time_tick.flag_time_count_stanby = 1;
    }
    else
    {
      Time_tick.count_time_1s++;
    }
    CT_ReadAllButton();
		ADC_Read_All(Adc_Arr_Convert);
  }
}

static void sys_status_init(void)
{
	sys_var.Status_sig = STT_INIT;
	CT_Relay(RELAY_ON);
	CT_BeeP(BEEP_OFF, 0);
	delay_ms(1500);
	sys_var.flag_normal_loading = 0;
}

static uint8_t sys_status_stanby(void)
{
	if(Time_tick.flag_time_count_stanby == 1)
	{
		Time_tick.flag_time_count_stanby = 0;
		if(sys_var.volt_out2 > 0)
		{
			sys_var.Status_sig = STT_STANBY;
			CT_Relay(RELAY_ON);
			CT_BeeP(BEEP_OFF, 0);
			sys_var.volt_out2 --;
			sys_var.flag_normal_loading = 0;
			return 0;
		}
		else
		{
			return 1;
		}
	}
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
/*
static void sys_control_motor(void)
{
	if(sys_var.volt_out1 > 220)
	{
		CT_Motor_1(1000,MOTOR_LEFT);
	}
	else if(sys_var.volt_out1 < 220)
	{
		CT_Motor_1(1000,MOTOR_RIGHT);
	}
	else 
	{
		CT_Motor_1(1000,MOTOR_OFF);
	}
	
	if(sys_var.volt_13m2 > VOLT13_AVG)
	{
		CT_Motor_2(1000,MOTOR_LEFT);
	}
	else if(sys_var.volt_13m2 < VOLT13_AVG)
	{
		CT_Motor_2(1000,MOTOR_RIGHT);
	}
	else 
	{
		CT_Motor_2(1000,MOTOR_OFF);
	}
}
*/
static void sys_spi_transmission(void)
{
	
}

static void sys_Test_motor(uint8_t mode)
{
	if(mode == 1)
	{
		if (BT1.Hold == 1)
		{
				//Send_String("3");
			CT_Motor_1(1000,MOTOR_LEFT);
		}
		else if (BT2.Hold == 1)
		{
				//Send_String("4");
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