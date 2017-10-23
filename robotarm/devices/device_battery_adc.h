#ifndef __DEVICE_BATTERY_ADC_H
#define __DEVICE_BATTERY_ADC_H
#include "stm32f10x.h"
#define BATTERY_ADC_Channel  15

uint16_t read_voltage(void);
void battery_adc_init(void);
#endif

