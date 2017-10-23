#include "device_mpu6050.h"
/*
  config the interrrupt pin of MPU6050
*/
void mpu6050_int_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
	GPIO_InitStruct.GPIO_Pin = MPU_INT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}

/*
  INT and bypass configure
*/
uint8_t mpu_int_bypass_config(uint8_t data)
{
  return senser_write_byte(MPU_ADDR,MPU_INTBP_CFG_REG,data);
}

/*
  whether configure to master mode
*/
uint8_t mpu_set_iic_master(uint8_t data)
{
	return senser_write_byte(MPU_ADDR,MPU_USER_CTRL_REG,data);
}
/*
  setting the sleep register
*/
uint8_t mpu_power_manegment(uint8_t data)
{
  return senser_write_byte(MPU_ADDR,MPU_PWR_MGMT1_REG,data);
}

//setting full scale range of gyroscope
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//return:0,successed
//others,failed
uint8_t mpu_set_gyro_fsr(uint8_t fsr)
{
	if((fsr==GYRO_FSR_250)||(fsr==GYRO_FSR_500)
	||(fsr==GYRO_FSR_1000)||(fsr==GYRO_FSR_2000))
	  return senser_write_byte(MPU_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
  else return 1; 
}

//setting full scale range of accelemeter
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//return:0,successed
//others,failed 
uint8_t mpu_set_accel_fsr(uint8_t fsr)
{
	if((fsr==ACCE_FSR_2_G)||(fsr==ACCE_FSR_4_G)
	 ||(fsr==ACCE_FSR_8_G)||(fsr==ACCE_FSR_16_G))
	return senser_write_byte(MPU_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围 
  else return 1;	
}

//setting digital low pass filter
//lpf:frequency of filter(Hz)
//return:0,setting successd
// others,setting failed 
uint8_t mpu_set_lpf(uint8_t lpf)
{
	if((lpf==LPF_BAND_WIDTH_256)||(lpf==LPF_BAND_WIDTH_188)
	 ||(lpf==LPF_BAND_WIDTH_98)||(lpf==LPF_BAND_WIDTH_42)
	 ||(lpf==LPF_BAND_WIDTH_20)||(lpf==LPF_BAND_WIDTH_10)
	 ||(lpf==LPF_BAND_WIDTH_5))
	return senser_write_byte(MPU_ADDR,MPU_CFG_REG,lpf);//设置数字低通滤波器  
  else return 1;
}

//setting sample rate of mpu6050 (suppose Fs=1KHz)
//rate:this value is range from 4~1000(Hz)
//return value:0,setting successd
//others,setting failed 
uint8_t mpu_set_sample_rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=senser_write_byte(MPU_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
	rate = rate/2;
	if(rate>=255) rate = LPF_BAND_WIDTH_256;
	else if(rate>=188)rate=LPF_BAND_WIDTH_188;
	else if(rate>=98)rate=LPF_BAND_WIDTH_98;
	else if(rate>=42)rate=LPF_BAND_WIDTH_42;
	else if(rate>=20)rate=LPF_BAND_WIDTH_20;
	else if(rate>=10)rate=LPF_BAND_WIDTH_10;
	else data=6;
 	return mpu_set_lpf(rate);	//自动设置LPF为采样率的一半
}


