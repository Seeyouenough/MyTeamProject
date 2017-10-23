#ifndef __CTRL_H
#define __CTRL_H
#include "stm32f10x.h"
#include "driver_led.h"
#include "driver_mpu6050.h"
#include "founction.h"
#include "imu.h"
#include "timer.h"
/* PID�����ṹ�� */
typedef struct
{
    float kp;
    float kd;
    float ki;
    float kdamp;
} pid_t;

/* PID������ö���� */
enum
{
    PIDROLL,
    PIDPITCH,
    PIDYAW,
    PID4,
    PID5,
    PID6,
    PIDITEMS
};

/* PID���ƽṹ�� */
typedef struct
{
    xyz_float_t err;
    xyz_float_t err_old;
    xyz_float_t err_i;
    xyz_float_t eliminate_I;
    xyz_float_t err_d;
    xyz_float_t damp;
    xyz_float_t out;
    pid_t 	PID[PIDITEMS];
    xyz_float_t err_weight;
    float FB;
} ctrl_t;

#endif
