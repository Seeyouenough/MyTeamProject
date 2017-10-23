#include "driver_led.h"
#include "delay.h"

void fc_led_init(void)
{
  led_init();
}

void led2_flash(int16_t time,int8_t times)
{
  uint8_t i;
	for(i=times;i>0;i--)
	{
		GPIO_ResetBits(GPIOA,LED2);
		delay_ms(time);
		GPIO_SetBits(GPIOA,LED2);
		delay_ms(time);
	}
}

void led3_flash(int16_t time,int8_t times)
{
  uint8_t i;
	for(i=times;i>0;i--)
	{
		GPIO_ResetBits(GPIOA,LED3);
		delay_ms(time);
		GPIO_SetBits(GPIOA,LED3);
		delay_ms(time);
	}
}

void run_flash(int16_t time,int8_t times)
{
  uint8_t i;
	for(i=times;i>0;i--)
	{
		GPIO_ResetBits(GPIOC,LED4);
		delay_ms(time);
		GPIO_SetBits(GPIOC,LED4);
		delay_ms(time);
	}
}

