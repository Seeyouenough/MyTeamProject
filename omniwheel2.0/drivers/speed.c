#include "speed.h"
#include "pwm.h"

void speed_interface_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_ICInitTypeDef  TIM3_ICInitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		    // 复用浮空输入
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);
	
	/* Time base configuration */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = 65535;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：不预分频，即为36MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge ,TIM_ICPolarity_BothEdge );//设置TIMx编码界面
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM1的更新标志位
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	TIM3->CNT = 0;
	
  TIM_Cmd(TIM3, ENABLE);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		    // 复用浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);//改变指定管脚的映射
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	/* Time base configuration */		 
	//TIM_TimeBaseStructure.TIM_Period = 24096;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 65535;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：不预分频，即为36MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 1;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	

	
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge ,TIM_ICPolarity_BothEdge );//设置TIMx编码界面
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIM1的更新标志位
                   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM2->CNT = 0;
  TIM_Cmd(TIM2, ENABLE);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		    // 复用浮空输入
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	
	 GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
	
	/* Time base configuration */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = 65535;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：不预分频，即为36MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 2;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge ,TIM_ICPolarity_BothEdge );//设置TIMx编码界面
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM1的更新标志位
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM4->CNT = 0;
	
  TIM_Cmd(TIM4, ENABLE);
}

void speed_measure(void)
{
	 motor1.speed= (short)TIM3 -> CNT;  TIM3 -> CNT=0;
	 motor2.speed= (short)TIM2 -> CNT;  TIM2 -> CNT=0;
	 motor3.speed= (short)TIM4 -> CNT;  TIM4 -> CNT=0;
}
