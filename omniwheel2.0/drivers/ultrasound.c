#include "ultrasound.h"
#include "imu.h"
#include "delay.h"

EXTI_InitTypeDef Ultra_EXTI_InitStructure;
NVIC_InitTypeDef Ultra_NVIC_InitStructure;
float ultra_angle,straight_angle;
uint8_t ultra_miss=1,ultra_index=0;
float ultra_buff[10],ultra_temp;
uint32_t ultra_cycle=0,ultra_cnt=0;
float ultra_distance=0;

void ultrasound_init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStructure;
		
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA端口时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟		
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	  GPIO_Init(GPIOC, &GPIO_InitStructure);					 
	  GPIO_ResetBits(GPIOC,GPIO_Pin_5);						
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PA6
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA6

  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);

  	Ultra_EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	Ultra_EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	Ultra_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
  	Ultra_EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&Ultra_EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
		Ultra_NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能按键所在的外部中断通道
  	Ultra_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级2， 
  	Ultra_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级1
  	Ultra_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&Ultra_NVIC_InitStructure); 
}

void set_ultra_edge(uint8_t edge)
{
  if(edge==1){
	  Ultra_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
		EXTI_Init(&Ultra_EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	}else if(edge==0){
	  Ultra_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
		EXTI_Init(&Ultra_EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	}
}


#define RISING_EDGE   0x00
#define FALLING_EDGE  0x01

void EXTI4_IRQHandler(void)
{
	static uint32_t falling_time,rising_time;
	static uint8_t ultra_state=RISING_EDGE,miss_cnt=0;
	uint8_t i;
	if(ultra_state==RISING_EDGE){ //捕获上升沿
	   rising_time = TIM1->CNT;
		 ultra_state = FALLING_EDGE;
		 set_ultra_edge(0);
		 ultra_cycle = 0; 
	}
	else if(ultra_state==FALLING_EDGE){
		  ultra_state = RISING_EDGE;
		 set_ultra_edge(1);
	   falling_time = TIM1->CNT;
		 if(ultra_cycle==0) {
		   ultra_distance = falling_time - rising_time;
		 }else{
		   ultra_distance = TIM1->ARR - rising_time + ultra_cycle*TIM1->ARR + falling_time;
		 }
		 ultra_distance = ultra_distance/58.0f;
		 ultra_cycle = 0; 
	}else {
	  ultra_cycle = 0;
		ultra_distance = 0; 
		ultra_state = RISING_EDGE;
	  set_ultra_edge(1);
	}
	
	ultra_buff[ultra_index] = ultra_distance;
	ultra_index = (ultra_index>=10)? 0:ultra_index++;
	for(i=0;i<10;i++){
	  ultra_temp += ultra_buff[i];
		if((ultra_buff[i]>2.0)&&(ultra_buff[i]<50)){
			ultra_cnt++; 
      if(ultra_cnt>MISS_WEIGHT_CNT) {ultra_angle = IMU_Yaw; ultra_miss = 0;}
			if(ultra_cnt>4000000) ultra_cnt=0;
		}else {
			miss_cnt++;  if(miss_cnt>=5){ultra_miss = 1;}
		}
	}
	ultra_temp /= 10.0f;
	
 	EXTI_ClearITPendingBit(EXTI_Line4);    //清除LINE5上的中断标志位  
}

void senor_loop(void)
{
  	ULTRASOUND_TRIG = 1;
		delay_us(10);
		ULTRASOUND_TRIG = 0;
}
uint32_t get_ultra_counter(void)
{
//	uint32_t temp;
//	temp = ultra_cnt;
  return ultra_cnt;
}


uint32_t ultra_sys_runtime=0;
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void ultra_timer_init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM1, //TIM1
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设
}

//void TIM1_UP_IRQHandler(void)   //TIM1中断
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
//		{
//			TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
//			ultra_sys_runtime++;
//			ultra_cycle++;
//			if(ultra_sys_runtime>=1000000){
//			  ultra_sys_runtime =0;
//			}
//		}
//}

uint32_t ultra_system_time(void)
{
   return ultra_sys_runtime;
}

