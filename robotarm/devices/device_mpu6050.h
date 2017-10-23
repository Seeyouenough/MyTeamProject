#ifndef __DEVICE_MPU6050_H
#define __DEVICE_MPU6050_H
#include "stm32f10x.h"
#include <stdbool.h>
#include "driver_senser_iic.h"
#include "delay.h"

//#define SELF_TEST_X  0x0D
//#define SELF_TEST_Y  0x0E
//#define SELF_TEST_Z  0x0F
//#define SELF_TEST_A  0x10
//#define SMPLRT_DIV   0x19
//#define CONFIG       0x1A
//#define GYRO_CONFIG  0x1B
//#define ACCE_CONFIG  0x1C
//#define MOT_THR      0x1F
//#define FIFO_EN      0x23

//#define I2C_MST_CTRL  0x24
//#define I2C_SLV0_ADDR 0x25
//#define I2C_SLV0_REG  0x26
//#define I2C_SLV0_CTRL 0x27
//#define I2C_SLV1_ADDR 0x28
//#define I2C_SLV1_REG  0x29
//#define I2C_SLV1_CTRL 0x2A

//#define I2C_MST_STATUS 0x36
//#define INT_PIN_CFG    0x37
//#define INT_ENABLE     0x38
//#define INT_STATUS     0x3A

//#define ACCEL_XOUT_H  0x3B
//#define ACCEL_XOUT_L  0x3C
//#define ACCEL_YOUT_H  0x3D
//#define ACCEL_YOUT_L  0x3E
//#define ACCEL_ZOUT_H  0x3F
//#define ACCEL_ZOUT_L  0x40

//#define TEMP_OUT_H    0x41
//#define TEMP_OUT_L    0x42
//#define GYRO_XOUT_H   0x43
//#define GYRO_XOUT_L   0x44
//#define GYRO_YOUT_H   0x45
//#define GYRO_YOUT_L   0x46
//#define GYRO_ZOUT_H   0x47
//#define GYRO_ZOUT_L   0x48

//#define EXT_SENS_DATA_00 0x49
//#define EXT_SENS_DATA_01 0x4A
//#define EXT_SENS_DATA_02 0x4B
//#define EXT_SENS_DATA_03 0x4C
//#define EXT_SENS_DATA_04 0x4D
//#define EXT_SENS_DATA_05 0x4E
//#define EXT_SENS_DATA_06 0x4F
//#define EXT_SENS_DATA_07 0x50
//#define EXT_SENS_DATA_08 0x51
//#define EXT_SENS_DATA_09 0x52
//#define EXT_SENS_DATA_10 0x53
//#define EXT_SENS_DATA_11 0x54
//#define EXT_SENS_DATA_12 0x55
//#define EXT_SENS_DATA_13 0x56
//#define EXT_SENS_DATA_14 0x57
//#define EXT_SENS_DATA_15 0x58
//#define EXT_SENS_DATA_16 0x59
//#define EXT_SENS_DATA_17 0x5A
//#define EXT_SENS_DATA_18 0x5B
//#define EXT_SENS_DATA_19 0x5C
//#define EXT_SENS_DATA_20 0x5D
//#define EXT_SENS_DATA_21 0x5E
//#define EXT_SENS_DATA_22 0x5F
//#define EXT_SENS_DATA_23 0x60

//#define I2C_SLV0_DO      0x63
//#define I2C_SLV1_DO      0x64

//#define I2C_MST_DELAY_CTRL 0x67
//#define SIGNAL_PATH_RESET  0x68
//#define MOT_DETECT_CTRL    0x69
//#define USER_CTRL          0x6A
//#define PWR_MGMT_1         0x6B
//#define PWR_MGMT_2         0x6C
//#define FIFO_COUNTH        0x72
//#define FIFO_COUNTL        0x73
//#define FIFO_R_W           0x74
//#define WHO_AM_I           0x75

//#define MPU_ACCEL_OFFS_REG		0X06	//accel_offs寄存器,可读取版本号,寄存器手册未提到
//#define MPU_PROD_ID_REG			0X0C	//prod id寄存器,在寄存器手册未提到
#define MPU_SELF_TESTX_REG		0X0D	//自检寄存器X
#define MPU_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU_SELF_TESTA_REG		0X10	//自检寄存器A
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU_CFG_REG				0X1A	//配置寄存器
#define MPU_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU_MOTION_DET_REG		0X1F	//运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG			0X23	//FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG		0X24	//IIC主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG	0X25	//IIC从机0器件地址寄存器
#define MPU_I2CSLV0_REG			0X26	//IIC从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG	0X27	//IIC从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG	0X28	//IIC从机1器件地址寄存器
#define MPU_I2CSLV1_REG			0X29	//IIC从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG	0X2A	//IIC从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG	0X2B	//IIC从机2器件地址寄存器
#define MPU_I2CSLV2_REG			0X2C	//IIC从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG	0X2D	//IIC从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG	0X2E	//IIC从机3器件地址寄存器
#define MPU_I2CSLV3_REG			0X2F	//IIC从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG	0X30	//IIC从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG	0X31	//IIC从机4器件地址寄存器
#define MPU_I2CSLV4_REG			0X32	//IIC从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG		0X33	//IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG	0X34	//IIC从机4控制寄存器
#define MPU_I2CSLV4_DI_REG		0X35	//IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG		0X36	//IIC主机状态寄存器
#define MPU_INTBP_CFG_REG		0X37	//中断/旁路设置寄存器
#define MPU_INT_EN_REG			0X38	//中断使能寄存器
#define MPU_INT_STA_REG			0X3A	//中断状态寄存器

#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG		0X3C	//加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG		0X3D	//加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG		0X3E	//加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG		0X3F	//加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG		0X40	//加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU_TEMP_OUTL_REG		0X42	//温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG		0X63	//IIC从机0数据寄存器
#define MPU_I2CSLV1_DO_REG		0X64	//IIC从机1数据寄存器
#define MPU_I2CSLV2_DO_REG		0X65	//IIC从机2数据寄存器
#define MPU_I2CSLV3_DO_REG		0X66	//IIC从机3数据寄存器

#define MPU_I2CMST_DELAY_REG	0X67	//IIC主机延时管理寄存器
#define MPU_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU_USER_CTRL_REG		0X6A	//用户控制寄存器
#define MPU_PWR_MGMT1_REG		0X6B	//电源管理寄存器1
#define MPU_PWR_MGMT2_REG		0X6C	//电源管理寄存器2 
#define MPU_FIFO_CNTH_REG		0X72	//FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG		0X73	//FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG			0X74	//FIFO读写寄存器
#define MPU_DEVICE_ID_REG		0X75	//器件ID寄存器
 
//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR				0X69


#define GYRO_FSR_250  0
#define GYRO_FSR_500  1
#define GYRO_FSR_1000 2
#define GYRO_FSR_2000 3
#define ACCE_FSR_2_G  0
#define ACCE_FSR_4_G  1
#define ACCE_FSR_8_G  2
#define ACCE_FSR_16_G 3

#define LPF_BAND_WIDTH_256 0x00
#define LPF_BAND_WIDTH_188 0x01
#define LPF_BAND_WIDTH_98  0x02
#define LPF_BAND_WIDTH_42  0x03
#define LPF_BAND_WIDTH_20  0x04
#define LPF_BAND_WIDTH_10  0x05
#define LPF_BAND_WIDTH_5   0x06
////因为模块AD0默认接GND,所以转为读写地址后,为0XD1和0XD0(如果接VCC,则为0XD3和0XD2)  
//#define MPU_READ    0XD1
//#define MPU_WRITE   0XD0


uint8_t mpu_power_manegment(uint8_t data);
uint8_t mpu_int_bypass_config(uint8_t data);
uint8_t mpu_set_iic_master(uint8_t data);
uint8_t mpu_set_gyro_fsr(uint8_t fsr);
uint8_t mpu_set_accel_fsr(uint8_t fsr);
uint8_t mpu_set_lpf(uint8_t lpf);
uint8_t mpu_set_sample_rate(uint16_t rate);
void mpu6050_int_config(void);

#define MPU_INT GPIO_Pin_10

#endif

