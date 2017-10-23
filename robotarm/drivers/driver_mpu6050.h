#ifndef __DRIVER_MPU6050_H
#define __DRIVER_MPU6050_H
#include "stm32f10x.h"
#include "device_mpu6050.h"
#include "driver_senser_iic.h"
#include "device_data_trans.h"
#include "founction.h"
#include "delay.h"

extern mpu6050_structure mpu6050;

uint8_t mpu_init(void);
void mpu6050_data_prepare(void);
void usart1_send_char(u8 c);
void usart1_niming_report(u8 fun,u8*data,u8 len);
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);
#endif

