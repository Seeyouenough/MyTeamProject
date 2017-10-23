#ifndef __LED_H
#define __LED_H	 
#include "sys.h"


//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PAout(8)	// PA8
#define LED1 PDout(2)	// PD2	

void led_init(void);//≥ı ºªØ
void led0_flash(uint8_t times,uint8_t time);
void led1_flash(uint8_t times,uint8_t time);
		 				    
#endif
