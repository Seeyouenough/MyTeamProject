#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"  


extern uint32_t sys_runtime;
extern uint8_t remote_times;
void timer_init(u16 arr,u16 psc); 
uint32_t system_time(void);
 
#endif
