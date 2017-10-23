#ifndef __DRIVER_LED_H
#define __DRIVER_LED_H
#include "stm32f10x.h"
#include "device_led.h"

void fc_led_init(void);
void led2_flash(int16_t time,int8_t times);
void led3_flash(int16_t time,int8_t times);
void run_flash(int16_t time,int8_t times);

#endif
