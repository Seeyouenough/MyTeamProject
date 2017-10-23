#include "stm32f10x.h"
#include "mpu9250.h"
#include "delay.h"
#include "timer.h"
#include "speed.h"
#include "led.h"
#include "pwm.h"
#include "ctrl.h"
#include "imu.h"
#include "flysky.h"
#include "usart.h"
#include "ultrasound.h"

int main(void)
{
	uint8_t err_code;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	led_init();
	pwm_init();
	usart_init(115200);
	speed_interface_init();

	while(1){
	  err_code = MPU9250_Init();
		if(err_code!=0){
		  printf("mpu dmp init err: %d\r\n",err_code);
			led0_flash(1,200);
		}else break;
	}
	timer_init(999,71);  //1ms
//	RC_Init();
//	ultrasound_init();
//	ultra_timer_init(71,0);
	ctrl_init();
	mpu6050.Acc_CALIBRATE = 1;
	mpu6050.Gyro_CALIBRATE = 1;
	Mag_CALIBRATED = true;
	
	led0_flash(4,200);
	
	while(1)
  {
		flysky_loop();  
//		senor_loop();
	}
}

