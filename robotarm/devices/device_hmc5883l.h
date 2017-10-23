#ifndef __DEVICE_HMC5883L_H
#define __DEVICE_HMC5883L_H
#include "stm32f10x.h"


#define HMC_REG_CONFIGA 0x00
#define HMC_REG_CONFIGB 0x01

#define HMC_REG_MODE    0x02

#define HMC_REG_X_MSB   0x03
#define HMC_REG_X_LSB   0x04
#define HMC_REG_Z_MSB   0x05
#define HMC_REG_Z_LSB   0x06
#define HMC_REG_Y_MSB   0x07
#define HMC_REG_Y_LSB   0x08

#define HMC_REG_STATUS  0x09
#define HMC_REG_IDA     0x0A
#define HMC_REG_IDB     0x0B
#define HMC_REG_IDC     0x0C  //Ê¶±ð¼Ä´æÆ÷


#define HMC_IDA_DEFAULT 0x48
#define HMC_IDB_DEFAULT 0x34
#define HMC_IDC_DEFAULT 0x33

#define HMC_DRDY  GPIO_Pin_9

void hmc_gpio_init(void);
#endif
