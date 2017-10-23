#ifndef __CTRL_H
#define __CTRL_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "timer.h"
#include "speed.h"
#include "mpu9250.h"
#include "flysky.h"

typedef enum
{
  user_mode,
	fiding_mode
}ctrl_mode;

typedef struct
{
  ctrl_mode mode;
}ctrl_structure;

extern ctrl_structure ctrl;

void ctrl_init(void);
void system_ctrl(void);
void set_toward(float m1,float m2,float m3);
void enable_motor(bool st1,bool st2,bool st3);
float speed_value(float target,float sample);
float turn_value(float err_angle,float gyroz);
void turn_around(void);
#endif

