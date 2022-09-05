#include "sys_lib.h"
#include "stdio.h"
static _time_t Time_tick;
static void SYS_TIM3_Init(void);
static uint16_t Adc_Arr_Convert[6];
static volatile  uint32_t data = 0, i;

volatile uint8_t tog = 0;

void SYS_Init_ALL(void)
{
	/*
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPISCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(SPI_CM_PORT, &GPIO_InitStructure);
	*/
  //UART_Init();
  //ADC_Init_All();
  //CT_Init();
  CT_Motor_Init();
  SYS_TIM3_Init();
  //SPI_Command_Init();
  
  
}

void SYS_Run(void)
{

  char arr[100];
  if (BT1.Flag == 1)
  {
    BT1.Flag = 0;
    //CT_BeeP(BEEP_ON, 300);
    sprintf(arr, "%d\n", 5);
    Send_String(arr);
  }

  if (BT2.Flag == 1)
  {
    BT2.Flag = 0;
    //CT_BeeP(BEEP_OFF, 300);
    Send_String("Off \n");
  }
  if (Adc_VarArr.adc_flag_ReadALL == 1)
  {
    Adc_VarArr.adc_flag_ReadALL = 0;
    sprintf(arr, "adc0:%d adc1:%d adc2:%d adc3:%d adc8:%d adc9:%d\n", Adc_Arr_Convert[0], Adc_Arr_Convert[1], Adc_Arr_Convert[2], Adc_Arr_Convert[3], Adc_Arr_Convert[4], Adc_Arr_Convert[5]);
    Send_String(arr);
  }

	//SPIx_EnableSlave();
	//SPIx_Transfer(0xAA);
	//SPIx_DisableSlave();
	//delay_ms(1000);
  //    for(i=0; i<10; i++)
  //    {
  //      data += ADC_Read_One_Channel(ADC_Channel_0_OUT_ADC_1, ADC_SampleTime_1_5Cycles);
  //    }
  //    sprintf(arr, "adc0:%d\n",(data/10));
  //    data = 0;
  //    Send_String(arr);
  //delay_ms(100);

		CT_Motor_1(500,MOTOR_LEFT);
		CT_Motor_2(700,MOTOR_LEFT);
  /*
    
    CT_Motor_2(100,MOTOR_OFF);
    SPIx_EnableSlave();
    SPIx_EnableSlave();
    SPIx_Transfer(0xABCD);
    SPIx_DisableSlave();
    delay_ms(1000);
  */
}

static void SYS_TIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = 996; //47955
  TIM_TimeBaseStructure.TIM_Prescaler = 8000;
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
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		GPIO_WriteBit(SPI_CM_PORT, SPISCK, tog);
		tog =! tog;
    if (Time_tick.count_time_1s >= 1000)
    {
      Time_tick.count_time_1s = 0;
      Time_tick.flag_time_1s = 1;
    }
    else
    {
      Time_tick.count_time_1s++;
    }
    CT_ReadAllButton();
    ADC_Read_All(Adc_Arr_Convert);
  }
}

