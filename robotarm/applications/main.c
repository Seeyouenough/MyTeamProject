#include <stdio.h>
#include "delay.h"
#include "stm32f10x.h"
#include "driver_led.h"
#include "driver_mpu6050.h"
#include "driver_data_trans.h"
#include "founction.h"
#include "device_capture.h"
#include "device_pwm.h"
#include "timer.h"
#include "imu.h"

int main()
{
	uint8_t error_code;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
  fc_led_init();
//	timer_init();
	motor_init(); //motor pwm init
	capture_init();
	trans_params_init(115200);
  while(1)
  {
		error_code=mpu_init();
		if(error_code){
			printf("mpu init error...\r\n");
			delay_ms(200);
		}else break;
  }
	set_pwm(0,0,0,0,0,0);
	
	while(1)
	{
		capture_filter();
		set_pwm(capture.roll.value,capture.pitch.value
		,capture.throttle.value,capture.yaw.value,capture.aux2.value,capture.aux3.value);
	}
}


