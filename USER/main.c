
#include "sys_lib.h"

int main(void) {
	
	SystemInit();
	SystemCoreClockUpdate();
	
	SYS_Init_ALL();
	
	while (1)
  { 
		SYS_Run();
	} 
}



