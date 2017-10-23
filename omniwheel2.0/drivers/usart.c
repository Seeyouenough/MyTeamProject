#include "sys.h"
#include "usart.h"	  

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
void usart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
  
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

  
}


#define USART1FrameLen  7
uint8_t rx_buff[USART1FrameLen];
uint16_t rx_ch[2];
uint8_t rx_deal=0;

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	static uint8_t index=0,sum=0;
	uint8_t recive;
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
			USART_ReceiveData(USART1);
	}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		recive = USART1->DR;
		if(rx_deal==0){
			if(index==0){//0��1 ��֡ͷ
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


