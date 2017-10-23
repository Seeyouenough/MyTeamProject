#ifndef __PWM_H
#define __PWM_H
#include <stdbool.h>
#include "sys.h"

typedef enum 
{
  postive,
	negtive
}motor_toward;

typedef struct
{
	bool  enable;
  short speed;
	uint16_t pwm;
	motor_toward toward;
	uint8_t ctrl_pin;
}motor_structure;


extern motor_structure motor1;
extern motor_structure motor2;
extern motor_structure motor3;

void pwm_init(void);
void set_pwm(uint16_t m1,uint16_t m2,uint16_t m3);
#endif
