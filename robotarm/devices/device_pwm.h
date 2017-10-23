#ifndef __DEVICE_PWM_H
#define __DEVICE_PWM_H
#include "stm32f10x.h"

void motor_init(void);
void set_pwm(uint16_t motor1,uint16_t motor2,uint16_t motor3,uint16_t motor4,uint16_t motor5,uint16_t motor6);
#endif

