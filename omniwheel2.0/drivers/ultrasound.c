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
		
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA�˿�ʱ��
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��		
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	  GPIO_Init(GPIOC, &GPIO_InitStructure);					 
	  GPIO_ResetBits(GPIOC,GPIO_Pin_5);						
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PA6
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA6

  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);

  	Ultra_EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	Ultra_EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	Ultra_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
  	Ultra_EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&Ultra_EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
		
		Ultra_NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	Ultra_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�2�� 
  	Ultra_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�1
  	Ultra_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&Ultra_NVIC_InitStructure); 
}

void set_ultra_edge(uint8_t edge)
{
  if(edge==1){
	  Ultra_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
		EXTI_Init(&Ultra_EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	}else if(edge==0){
	  Ultra_EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
		EXTI_Init(&Ultra_EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	}
}


#define RISING_EDGE   0x00
#define FALLING_EDGE  0x01

void EXTI4_IRQHandler(void)
{
	static uint32_t falling_time,rising_time;
	static uint8_t ultra_state=RISING_EDGE,miss_cnt=0;
	uint8_t i;
	if(ultra_state==RISING_EDGE){ //����������
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
	
 	EXTI_ClearITPendingBit(EXTI_Line4);    //���LINE5�ϵ��жϱ�־λ  
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
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void ultra_timer_init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM1, //TIM1
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx����
}

//void TIM1_UP_IRQHandler(void)   //TIM1�ж�
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
//		{
//			TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
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

