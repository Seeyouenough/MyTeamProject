#include "timer.h"
#include "led.h"
#include "imu.h"
#include "mpu9250.h"
#include "delay.h"
#include "ultrasound.h"
#include "ctrl.h"

uint32_t sys_runtime=0;
uint8_t  remote_times;

void timer_init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM7, //TIM7
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM7, ENABLE);  //使能TIMx外设
}

void TIM7_IRQHandler(void)   //TIM7中断
{
			if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
			{
				sys_runtime++;
	//			ultra_cycle++;
				if(sys_runtime>10000000) sys_runtime=0;
				if(sys_runtime%2==0) {
					Call_MPU6050_Data_Prepare(0.002);
				}
				if(sys_runtime%10==0) Call_AK8975();
				if(sys_runtime%5==0) Call_IMUupdate(0.0025);
				if(sys_runtime%10==0) {
					system_ctrl();
				}
				TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
		  }
}






uint32_t system_time(void)
{
   return sys_runtime;
}


















