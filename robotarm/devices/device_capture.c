#include "device_capture.h"

uint8_t  TIM4_CH1_STA=0,TIM4_CH2_STA=0,TIM4_CH3_STA=0,TIM4_CH4_STA=0;	//输入捕获状态		    				
uint16_t TIM4CH1_CAPTURE_VAL1,TIM4CH2_CAPTURE_VAL1,TIM4CH3_CAPTURE_VAL1,TIM4CH4_CAPTURE_VAL1;	//输入捕获值
uint16_t TIM4CH1_CAPTURE_VAL2,TIM4CH2_CAPTURE_VAL2,TIM4CH3_CAPTURE_VAL2,TIM4CH4_CAPTURE_VAL2;	//输入捕获值


uint8_t  TIM5_CH1_STA=0,TIM5_CH2_STA=0,TIM5_CH3_STA=0,TIM5_CH4_STA=0;	//输入捕获状态		    				
uint16_t TIM5CH1_CAPTURE_VAL1,TIM5CH2_CAPTURE_VAL1,TIM5CH3_CAPTURE_VAL1,TIM5CH4_CAPTURE_VAL1;	//输入捕获值
uint16_t TIM5CH1_CAPTURE_VAL2,TIM5CH2_CAPTURE_VAL2,TIM5CH3_CAPTURE_VAL2,TIM5CH4_CAPTURE_VAL2;	//输入捕获值


capture_ch_t capture;

void capture_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef  TIM4_ICInitStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM4时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//使能TIM5时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  //使能AFIO功能的时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
	
	//初始化定时器4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = 0xffff; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化定时器4 TIM5	 
	TIM_TimeBaseStructure.TIM_Period = 0xffff; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4输入捕获参数
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	
	//初始化TIM5输入捕获参数
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CCIE捕获中断	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CCIE捕获中断	
  TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
	TIM_Cmd(TIM5,ENABLE ); 	//使能定时器5
}

 
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 
	static uint8_t cnt[4];
	capture.pitch.dis=0;capture.roll.dis=0;capture.throttle.dis=0;capture.yaw.dis=0;
 	//如果得到的是捕获1中断
  if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)
  {
     TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);
     capture.throttle.dis=1;		
		 cnt[0]=0;
		 if(TIM4_CH1_STA & POSEDGE_CAPED)
		 {
			  TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		    TIM4CH1_CAPTURE_VAL2=TIM_GetCapture1(TIM4);
			  TIM4_CH1_STA=NEGEDGE_CAPED;
			  if(TIM4CH1_CAPTURE_VAL2>TIM4CH1_CAPTURE_VAL1)
				{
					capture.throttle.value=TIM4CH1_CAPTURE_VAL2-TIM4CH1_CAPTURE_VAL1;
				}
				else 
				{
				  capture.throttle.value=65535-TIM4CH1_CAPTURE_VAL1+TIM4CH1_CAPTURE_VAL2;
				}
		 }
		 else//第一次捕获到高电平
		 {
			 TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);
			 //捕获到第一个上升沿
			 TIM4_CH1_STA=POSEDGE_CAPED;
			 TIM4CH1_CAPTURE_VAL1=TIM_GetCapture1(TIM4);
			 //改为下降沿捕获
		 }		 
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC2)!=RESET)
  {  
		 TIM_ClearITPendingBit(TIM4,TIM_IT_CC2);
     capture.roll.dis=1;
		 cnt[1]=0;
		 if(TIM4_CH2_STA & POSEDGE_CAPED)
		 {
		    TIM4CH2_CAPTURE_VAL2=TIM_GetCapture2(TIM4);
			  TIM4_CH2_STA=NEGEDGE_CAPED;
			  if(TIM4CH2_CAPTURE_VAL2>TIM4CH2_CAPTURE_VAL1)
				{
					capture.roll.value=TIM4CH2_CAPTURE_VAL2-TIM4CH2_CAPTURE_VAL1;
				}
				else 
				{
				  capture.roll.value=65535-TIM4CH2_CAPTURE_VAL1+TIM4CH2_CAPTURE_VAL2;
				}
			  TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//第一次捕获到高电平
		 {
			 //捕获到第一个上升沿
			 TIM4_CH2_STA=POSEDGE_CAPED;
			 TIM4CH2_CAPTURE_VAL1=TIM_GetCapture2(TIM4);
			 //改为下降沿捕获
			 TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 } 
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC3)!=RESET)
  {  
		 TIM_ClearITPendingBit(TIM4,TIM_IT_CC3);
		 capture.pitch.dis=1;
		 cnt[2]=0;
		 if(TIM4_CH3_STA & POSEDGE_CAPED)
		 {
		    TIM4CH3_CAPTURE_VAL2=TIM_GetCapture3(TIM4);
			  TIM4_CH3_STA=NEGEDGE_CAPED;
			  if(TIM4CH3_CAPTURE_VAL2>TIM4CH3_CAPTURE_VAL1)
				{
					capture.pitch.value=TIM4CH3_CAPTURE_VAL2-TIM4CH3_CAPTURE_VAL1;
				}
				else 
				{
				  capture.pitch.value=65535-TIM4CH3_CAPTURE_VAL1+TIM4CH3_CAPTURE_VAL2;
				}
			  TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//第一次捕获到高电平
		 {
			 //捕获到第一个上升沿
			 TIM4_CH3_STA=POSEDGE_CAPED;
			 TIM4CH3_CAPTURE_VAL1=TIM_GetCapture3(TIM4);
			 //改为下降沿捕获
			 TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 } 
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
  {  
		 TIM_ClearITPendingBit(TIM4,TIM_IT_CC4);
		 capture.yaw.dis=1;
		 cnt[3]=0;
		 if(TIM4_CH4_STA & POSEDGE_CAPED)
		 {
		    TIM4CH4_CAPTURE_VAL2=TIM_GetCapture4(TIM4);
			  TIM4_CH4_STA=NEGEDGE_CAPED;
			  if(TIM4CH4_CAPTURE_VAL2>TIM4CH4_CAPTURE_VAL1)
				{
					capture.yaw.value=TIM4CH4_CAPTURE_VAL2-TIM4CH4_CAPTURE_VAL1;
				}
				else 
				{
				  capture.yaw.value=65535-TIM4CH4_CAPTURE_VAL1+TIM4CH4_CAPTURE_VAL2;
				}
			  TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//第一次捕获到高电平
		 {
			 //捕获到第一个上升沿
			 TIM4_CH4_STA=POSEDGE_CAPED;
			 TIM4CH4_CAPTURE_VAL1=TIM_GetCapture4(TIM4);
			 //改为下降沿捕获
			 TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 } 
	}
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//更新中断用于检测是否断开
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		if(capture.throttle.dis!=false){ capture.throttle.value = capture.throttle.value; }else{
			  cnt[0] ++; if(cnt[0]>200){
			  capture.throttle.value = 1000;
				
			 }
		}
    if(capture.roll.dis!=0){ capture.roll.value = capture.roll.value; }else {
			 cnt[1] ++; if(cnt[1]>200)
		   capture.roll.value = 1000;
		}
    if(capture.pitch.dis!=0){ capture.pitch.value = capture.pitch.value; }else{
			 cnt[2] ++; if(cnt[2]>200)
		   capture.pitch.value = 1000;
		}
		if(capture.yaw.dis!=0){ capture.yaw.value =  capture.yaw.value; }else{
			  cnt[3] ++; if(cnt[3]>200)
		    capture.yaw.value = 1000;
		}
	}
}

//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 
	static uint8_t cnt[4];
	capture.aux3.dis=0;capture.aux2.dis=0;capture.aux1.dis=0;capture.aux4.dis=0;
 	//如果得到的是捕获1中断
  if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)
  {
     TIM_ClearITPendingBit(TIM5,TIM_IT_CC1);
     capture.aux1.dis=1;		
		 cnt[0]=0;
		 if(TIM5_CH1_STA & POSEDGE_CAPED)
		 {
			  TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); 
		    TIM5CH1_CAPTURE_VAL2=TIM_GetCapture1(TIM5);
			  TIM5_CH1_STA=NEGEDGE_CAPED;
			  if(TIM5CH1_CAPTURE_VAL2>TIM5CH1_CAPTURE_VAL1)
				{
					capture.aux1.value=TIM5CH1_CAPTURE_VAL2-TIM5CH1_CAPTURE_VAL1;
				}
				else 
				{
				  capture.aux1.value=65535-TIM5CH1_CAPTURE_VAL1+TIM5CH1_CAPTURE_VAL2;
				}
		 }
		 else//第一次捕获到高电平
		 {
			 TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);
			 //捕获到第一个上升沿
			 TIM5_CH1_STA=POSEDGE_CAPED;
			 TIM5CH1_CAPTURE_VAL1=TIM_GetCapture1(TIM5);
			 //改为下降沿捕获
		 }		 
	}
	if(TIM_GetITStatus(TIM5,TIM_IT_CC2)!=RESET)
  {  
		 TIM_ClearITPendingBit(TIM5,TIM_IT_CC2);
     capture.aux2.dis=1;
		 cnt[1]=0;
		 if(TIM5_CH2_STA & POSEDGE_CAPED)
		 {
		    TIM5CH2_CAPTURE_VAL2=TIM_GetCapture2(TIM5);
			  TIM5_CH2_STA=NEGEDGE_CAPED;
			  if(TIM5CH2_CAPTURE_VAL2>TIM5CH2_CAPTURE_VAL1)
				{
					capture.aux2.value=TIM5CH2_CAPTURE_VAL2-TIM5CH2_CAPTURE_VAL1;
				}
				else 
				{
				  capture.aux2.value=65535-TIM5CH2_CAPTURE_VAL1+TIM5CH2_CAPTURE_VAL2;
				}
			  TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); 
		 }
		 else//第一次捕获到高电平
		 {
			 //捕获到第一个上升沿
			 TIM5_CH2_STA=POSEDGE_CAPED;
			 TIM5CH2_CAPTURE_VAL1=TIM_GetCapture2(TIM5);
			 //改为下降沿捕获
			 TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Falling);
		 } 
	}
	if(TIM_GetITStatus(TIM5,TIM_IT_CC3)!=RESET)
  {  
		 TIM_ClearITPendingBit(TIM5,TIM_IT_CC3);
		 capture.aux3.dis=1;
		 cnt[2]=0;
		 if(TIM5_CH3_STA & POSEDGE_CAPED)
		 {
		    TIM5CH3_CAPTURE_VAL2=TIM_GetCapture3(TIM5);
			  TIM5_CH3_STA=NEGEDGE_CAPED;
			  if(TIM5CH3_CAPTURE_VAL2>TIM5CH3_CAPTURE_VAL1)
				{
					capture.aux3.value=TIM5CH3_CAPTURE_VAL2-TIM5CH3_CAPTURE_VAL1;
				}
				else 
				{
				  capture.aux3.value=65535-TIM5CH3_CAPTURE_VAL1+TIM5CH3_CAPTURE_VAL2;
				}
			  TIM_OC3PolarityConfig(TIM5,TIM_ICPolarity_Rising); 
		 }
		 else//第一次捕获到高电平
		 {
			 //捕获到第一个上升沿
			 TIM5_CH3_STA=POSEDGE_CAPED;
			 TIM5CH3_CAPTURE_VAL1=TIM_GetCapture3(TIM5);
			 //改为下降沿捕获
			 TIM_OC3PolarityConfig(TIM5,TIM_ICPolarity_Falling);
		 } 
	}
	if(TIM_GetITStatus(TIM5,TIM_IT_CC4)!=RESET)
  {  
		 TIM_ClearITPendingBit(TIM5,TIM_IT_CC4);
		 capture.aux4.dis=1;
		 cnt[3]=0;
		 if(TIM5_CH4_STA & POSEDGE_CAPED)
		 {
		    TIM5CH4_CAPTURE_VAL2=TIM_GetCapture4(TIM5);
			  TIM5_CH4_STA=NEGEDGE_CAPED;
			  if(TIM5CH4_CAPTURE_VAL2>TIM5CH4_CAPTURE_VAL1)
				{
					capture.aux4.value=TIM5CH4_CAPTURE_VAL2-TIM5CH4_CAPTURE_VAL1;
				}
				else 
				{
				  capture.aux4.value=65535-TIM5CH4_CAPTURE_VAL1+TIM5CH4_CAPTURE_VAL2;
				}
			  TIM_OC4PolarityConfig(TIM5,TIM_ICPolarity_Rising); 
		 }
		 else//第一次捕获到高电平
		 {
			 //捕获到第一个上升沿
			 TIM5_CH4_STA=POSEDGE_CAPED;
			 TIM5CH4_CAPTURE_VAL1=TIM_GetCapture4(TIM5);
			 //改为下降沿捕获
			 TIM_OC4PolarityConfig(TIM5,TIM_ICPolarity_Falling);
		 } 
	}
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//更新中断用于检测是否断开
	{
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
		if(capture.aux1.dis!=false){ capture.aux1.value = capture.aux1.value; }else{
			  cnt[0] ++; if(cnt[0]>200){
			  capture.aux1.value = 1000;
				
			 }
		}
    if(capture.aux2.dis!=0){ capture.aux2.value = capture.aux2.value; }else {
			 cnt[1] ++; if(cnt[1]>200)
		   capture.aux2.value = 1000;
		}
    if(capture.aux3.dis!=0){ capture.aux3.value = capture.aux3.value; }else{
			 cnt[2] ++; if(cnt[2]>200)
		   capture.aux3.value = 1000;
		}
		if(capture.aux4.dis!=0){ capture.aux4.value =  capture.aux4.value; }else{
			  cnt[3] ++; if(cnt[3]>200)
		    capture.aux4.value = 1000;
		}
	}
}

void capture_filter(void)
{
  if(((capture.roll.value>=1475)&&(capture.roll.value<=1525))
   ||(capture.roll.value<980)||(capture.roll.value>2100)
   ){capture.roll.value=1500;}
	if((capture.roll.value<1000)&&(capture.roll.value>980)) {capture.roll.value=1000;}
	if((capture.roll.value>2000)&&(capture.roll.value<2100)) {capture.roll.value=2000;}
	
	if(((capture.pitch.value>=1475)&&(capture.pitch.value<=1525))
   ||(capture.pitch.value<980)||(capture.pitch.value>2100)
   ){capture.pitch.value=1500;}
	if((capture.pitch.value<1000)&&(capture.pitch.value>980)) {capture.pitch.value=1000;}
	if((capture.pitch.value>2000)&&(capture.pitch.value<2100)) {capture.pitch.value=2000;}
	
	if(((capture.yaw.value>=1475)&&(capture.yaw.value<=1525))
   ||(capture.yaw.value<980)||(capture.yaw.value>2100)
   ){capture.yaw.value=1500;}
	if((capture.yaw.value<1000)&&(capture.yaw.value>980)) {capture.yaw.value=1000;}
	if((capture.yaw.value>2000)&&(capture.yaw.value<2100)) {capture.yaw.value=2000;}
	
	if((capture.throttle.value<980)||(capture.throttle.value>2100)
   ){capture.throttle.value=1000;}
	if((capture.throttle.value<1000)&&(capture.throttle.value>980)) {capture.throttle.value=1000;}
	if((capture.throttle.value>2000)&&(capture.throttle.value<2100)) {capture.throttle.value=2000;}
	
	if((capture.aux1.value<980)||(capture.aux1.value>2100)
   ){capture.aux1.value=1000;}
	if((capture.aux1.value<1000)&&(capture.aux1.value>980)) {capture.aux1.value=1000;}
	if((capture.aux1.value>2000)&&(capture.aux1.value<2100)) {capture.aux1.value=2000;}
	
	if((capture.aux2.value<980)||(capture.aux2.value>2100)
   ){capture.aux2.value=1000;}
	if((capture.aux2.value<1000)&&(capture.aux2.value>980)) {capture.aux2.value=1000;}
	if((capture.aux2.value>2000)&&(capture.aux2.value<2100)) {capture.aux2.value=2000;}
	
	if((capture.aux3.value<980)||(capture.aux3.value>2100)
   ){capture.aux3.value=1000;}
	if((capture.aux3.value<1000)&&(capture.aux3.value>980)) {capture.aux3.value=1000;}
	if((capture.aux3.value>2000)&&(capture.aux3.value<2100)) {capture.aux3.value=2000;}
	
	if((capture.aux4.value<980)||(capture.aux4.value>2100)
   ){capture.aux4.value=1000;}
	if((capture.aux4.value<1000)&&(capture.aux4.value>980)) {capture.aux4.value=1000;}
	if((capture.aux4.value>2000)&&(capture.aux4.value<2100)) {capture.aux4.value=2000;}
}

