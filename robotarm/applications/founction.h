#ifndef __FOUNCTION_H
#define __FOUNCTION_H
#include "stm32f10x.h"
#include <stdbool.h>

#define LIMIT(value,min,max) (value<=min)?min:((value>=max) ? max:value)

typedef struct{
  uint16_t value;
	bool  dis;
}radio_channel_t;

typedef struct {
  radio_channel_t roll;
	radio_channel_t pitch;
	radio_channel_t yaw;
	radio_channel_t throttle;
	radio_channel_t aux1;
	radio_channel_t aux2;
	radio_channel_t aux3;
	radio_channel_t aux4;
}capture_ch_t;

typedef struct{
  float x;
	float y;
	float z;
}xyz_float_t;

typedef struct{
  int16_t x;
  int16_t y;
  int16_t z;  
}xyz_int16_t;

typedef struct{
	/*
	 calibrate flag bit
	*/
  bool accel_calibrate;
	bool gyro_calibrate;
	/*
	 mpu senser structure
	*/
	xyz_int16_t accel_adc; //adc value 16bits integer number
	xyz_int16_t gyro_adc;
	xyz_float_t accel;
	xyz_float_t gyro;
	xyz_float_t gyro_degree;
	/*
	 sensor data offset
	*/
	xyz_float_t accel_offset;
	xyz_float_t gyro_offset;
	
	/*tempreature*/
	int16_t tempreture_adc;
	float tempreture_lpf;
	float tempreture;
}mpu6050_structure;
#endif
