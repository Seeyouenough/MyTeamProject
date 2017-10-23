#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
extern uint16_t rx_ch[2];

void usart_init(u32 bound);
void usart_frame_deal(void);
void send_frame(uint8_t *str,uint8_t len);
#endif
