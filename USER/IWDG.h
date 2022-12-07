
/*
  ******************************************************************************
  * @file		IWDG.h                                                             *
  * @author	Nguyen Quang Bien                                                	 *
  * @date		1/1/2019                                                       		 *
  ******************************************************************************
*/
	
#ifndef __IWDG__
#define __IWDG__

#ifdef __cplusplus
 extern "C" {
#endif
#include "sys_lib.h"
#define LsiFreq (40000)
	 
void IWD_Init(float time_out);
void IWD_Clear_Time_Out(void);
 
#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/
