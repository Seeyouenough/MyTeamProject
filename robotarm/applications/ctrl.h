#ifndef __CTRL_H
#define __CTRL_H
#include "stm32f10x.h"
#include "driver_led.h"
#include "driver_mpu6050.h"
#include "founction.h"
#include "imu.h"
#include "timer.h"
/* PID参数结构体 */
typedef struct
{
    float kp;
    float kd;
    float ki;
    float kdamp;
} pid_t;

/* PID控制轴枚举体 */
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

/* PID控制结构体 */
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
