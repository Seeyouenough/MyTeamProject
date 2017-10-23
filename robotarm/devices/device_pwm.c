#include "device_pwm.h"
#include "founction.h"


/*
   initialize the pwm to driving ESC(eletronic speed controller)
*/
void motor_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);// enable the clock of TIMER1 and TIMER3
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);  //enable GPIO clock      
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM3_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //TIM3_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //TIM1_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = 19999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =71; //设置用来作为TIMx时钟频率除数的预分频值  72分频 1us
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
  
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
 
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; 
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1预装载使能	 

	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH2预装载使能	 
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH2预装载使能	
	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH3预装载使能	 
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH3预装载使能	

	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH4预装载使能	 
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH4预装载使能	
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	
  TIM_CtrlPWMOutputs(TIM3,ENABLE);	//MOE 主输出使能	
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
  TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}

#include "stdio.h"
void set_pwm(uint16_t motor1,uint16_t motor2,uint16_t motor3,uint16_t motor4,uint16_t motor5,uint16_t motor6)
{
	static float m1=450,m2=1100,m3=0,m4=0,m5=0,m6=0;
	
	
	if((motor1>=1420)&&(motor1<=1530)) motor1=1500;
	if((motor2>=1420)&&(motor2<=1530)) motor2=1500;
	printf("%6d %6d\r\n",motor3,motor4);
	
	m1 -= (((float)motor1 - 1500.0f)/150.0f);
	m2 -= (((float)motor2 - 1500.0f)/150.0f);
//	printf("%12lf %12lf\r\n",((float)motor1 - 1500.0f)/100.0f,((float)motor2 - 1500.0f)/100.0f);
	m3 = (uint16_t)(2600.0f*(float)(motor3 - 1000)/1000.0f);
	m4 = (uint16_t)(2600.0f*(float)(motor4 - 1000)/1000.0f);
	m5 = (uint16_t)(2600.0f*(float)(motor5 - 1000)/1000.0f);
	m6 = (uint16_t)(2600.0f*(float)(motor6 - 1000)/1000.0f);
	
	m1=motor1 = LIMIT(m1,450,2600);
	m2=motor2 = LIMIT(m2,1100,2600);
	m3=motor3 = LIMIT(m3,450,2600);
	m4=motor4 = LIMIT(m4,450,2600);
	m5=motor5 = LIMIT(m5,450,2600);
	m6=motor6 = LIMIT(m6,820,2000);
//	
//	printf("m1:%6d m2:%6d\r\n",motor1,motor2);
	
//	printf("%6d %6d %6d %6d %6d %6d\r\n",motor1,motor2,motor3,motor4,motor5,motor6);
//  TIM_SetCompare1(TIM3,motor1);
//	TIM_SetCompare2(TIM3,motor2);
//	TIM_SetCompare3(TIM3,motor3);
//	TIM_SetCompare4(TIM3,motor4);
	TIM3->CCR1 = motor1;
	TIM3->CCR2 = motor2;
	TIM3->CCR3 = motor3;
	TIM3->CCR4 = motor4;
	TIM1->CCR1 = motor5;
	TIM1->CCR2 = motor6;
//	TIM_SetCompare1(TIM1,motor5);
//	TIM_SetCompare2(TIM1,motor6);
//	TIM_SetCompare3(TIM1,motor3);
//	TIM_SetCompare4(TIM1,motor4);
}



