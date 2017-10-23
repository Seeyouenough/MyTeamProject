#ifndef __DRIVER_SENSER_H
#define __DRIVER_SENSER_H
#include "stm32f10x.h"

#define SENSER_IIC_SDA  GPIO_Pin_11
#define SENSER_IIC_SCL  GPIO_Pin_12

//IO·½ÏòÉèÖÃ
#define SENSER_SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define SENSER_SDA_OUT()     {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}
#define SENSER_READ_SDA()  GPIO_ReadInputDataBit(GPIOC,SENSER_IIC_SDA)


void iic_start(void);
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);
void iic_send_byte(uint8_t txd);
uint8_t iic_read_byte(unsigned char ack);

void senser_iic_init(void);
uint8_t senser_write_byte(uint8_t dev_addr,uint8_t reg_addr,uint8_t data);
uint8_t senser_read_byte(uint8_t dev_addr,uint8_t reg_addr);
uint8_t senser_read_len(uint8_t dev_addr,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t senser_write_len(uint8_t dev_addr,uint8_t reg,uint8_t len,uint8_t *buf);
#endif
