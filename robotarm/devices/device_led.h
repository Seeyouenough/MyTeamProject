#ifndef __DEVICE_LED_H
#define __DEVICE_LED_H
#include "stm32f10x.h"

#define LED2 GPIO_Pin_4
#define LED3 GPIO_Pin_5
#define LED4 GPIO_Pin_4

void led_init(void);

#endif
