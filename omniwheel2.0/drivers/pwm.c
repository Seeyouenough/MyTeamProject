#include "pwm.h"
#include "sys.h"

motor_structure motor1;
motor_structure motor2;
motor_structure motor3;


void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设时钟使能
	                                                                     	

   //设置该引脚为复用输出功能,输出TIM5 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2; //TIM_CH5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);

	TIM_TimeBaseStructure.TIM_Period = 7199; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =71; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx



  TIM_CtrlPWMOutputs(TIM5,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);  //CH1预装载使能	
  TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  //CH2预装载使能	
  TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //CH3预装载使能	
	
	TIM_ARRPreloadConfig(TIM5, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM5, ENABLE);  //使能TIM5
	
	motor1.pwm=0; motor1.enable = false; motor1.toward = postive; motor1.ctrl_pin = 12;
	motor2.pwm=0; motor2.enable = false; motor2.toward = postive; motor2.ctrl_pin = 13;
	motor3.pwm=0; motor3.enable = false; motor3.toward = postive; motor3.ctrl_pin = 14;
	set_pwm(0,0,0);
}

void set_pwm(uint16_t m1,uint16_t m2,uint16_t m3)
{
  if(motor1.enable==true){
		if(m1>7199)  m1 = 7199;
	  if(motor1.toward==postive){
		  PBout(motor1.ctrl_pin) = 0;
			TIM_SetCompare1(TIM5,m1);
		}else{
		  PBout(motor1.ctrl_pin) = 1;
			m1 = 7199 - m1;
			TIM_SetCompare1(TIM5,m1);
		}
	}else{
		PBout(motor1.ctrl_pin) = 0;
	  TIM_SetCompare1(TIM5,0);
	}
	if(motor2.enable==true){
		if(m2>7199)  m2 = 7199;
	  if(motor2.toward==postive){
		  PBout(motor2.ctrl_pin) = 0;
			TIM_SetCompare2(TIM5,m2);
		}else{
		  PBout(motor2.ctrl_pin) = 1;
			m2 = 7199 - m2;
			TIM_SetCompare2(TIM5,m2);
		}
	}else{
		PBout(motor2.ctrl_pin) = 0;
	  TIM_SetCompare2(TIM5,0);
	}
	if(motor3.enable==true){
		if(m3>7199)  m3 = 7199;
	  if(motor3.toward==postive){
		  PBout(motor3.ctrl_pin) = 0;
			TIM_SetCompare3(TIM5,m3);
		}else{
		  PBout(motor3.ctrl_pin) = 1;
			m3 = 7199 - m3;
			TIM_SetCompare3(TIM5,m3);
		}
	}else{
		PBout(motor3.ctrl_pin) = 0;
	  TIM_SetCompare3(TIM5,0);
	}
}
