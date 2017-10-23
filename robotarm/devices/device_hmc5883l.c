#include "device_hmc5883l.h"

void hmc_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //…œ¿≠ ‰»Î
	GPIO_InitStruct.GPIO_Pin = HMC_DRDY;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}
	

