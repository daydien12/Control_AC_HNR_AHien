

#ifndef _DELAY_
#define _DELAY_


#ifdef __cplusplus
extern "C"{
#endif
	
void delay_ms (uint16_t del);

void delay_us (uint16_t del);
	
	
void Fn_DELAY_Init (unsigned char _CLK);
void Fn_DELAY_us (unsigned long _vrui_Time);
void Fn_DELAY_ms (unsigned int _vrui_Time);

#ifdef __cplusplus
}
#endif

#endif


