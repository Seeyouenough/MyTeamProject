#include "sys.h"
#include "usart.h"	  

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
void usart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
  
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

  
}


#define USART1FrameLen  7
uint8_t rx_buff[USART1FrameLen];
uint16_t rx_ch[2];
uint8_t rx_deal=0;

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	static uint8_t index=0,sum=0;
	uint8_t recive;
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
			USART_ReceiveData(USART1);
	}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		recive = USART1->DR;
		if(rx_deal==0){
			if(index==0){//0，1 是帧头
				 if(recive==0x69) {
					 index++;  rx_buff[0] = recive;
				 }else {
					 index=0;  rx_buff[0] = 0x00;
				 }
			}else if(index==1){
				 if(recive==0x7f){
					 index++;  rx_buff[1] = recive;
				 }else{
					 index++;  rx_buff[1] = 0x00;
				 }
			}else{
				 rx_buff[index] = recive;
				 if(index==USART1FrameLen-1){
					 index=0;
					 sum = rx_buff[2]+rx_buff[3]+rx_buff[4]+rx_buff[5];
					 if(rx_buff[USART1FrameLen-1]==sum){
					   rx_deal=1;
						 usart_frame_deal();
					 }else{
					   rx_deal=0;
					 }
					 sum = 0;
				 }else{
					 index++;
				 }
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}

void usart_frame_deal(void)
{
  if(rx_deal==1){
		rx_deal=0;
		if((rx_buff[0]==0x69)&&(rx_buff[1]==0x7f)){
			rx_ch[0] = (rx_buff[2]<<8)|rx_buff[3];
			rx_ch[1] = (rx_buff[4]<<8)|rx_buff[5];
//			printf("%6d %6d\r\n",rx_ch[0],rx_ch[1]);
//      printf("frame recived...\r\n");
		}
		rx_buff[0]=rx_buff[1]=0x00;
		rx_buff[USART1FrameLen-1] = 0x00;
	}
}


