#include "driver_data_trans.h"
#include "founction.h"
#include "device_capture.h"
#include "device_pwm.h"

data_trans_frame tx_frame;
data_trans_frame rx_frame;


void trans_params_init(uint32_t bound)
{
  uint8_t i;
	rx_frame.head1 = 0x00;
	rx_frame.head2 = 0x00;
	rx_frame.fonc = 0x00;
	for(i=0;i<TransBuffSize;i++){rx_frame.data[i] =0x00;}
	rx_frame.is_ack = false;
	rx_frame.len = 0;
	rx_frame.sta = 0;
	transfer_init(bound);
}

void USART3_IRQHandler(void) 
{
	uint8_t recive;
	static uint8_t  index=0;
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
			USART_ReceiveData(USART1);
	}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		recive = USART3->DR;
		if(index==0){
		 if(recive==0x69){
		  index++;
		 }else{
		  index=0;
		 }
		}else if(index==1){
		 if(recive==0x7f){
		  index++;
		 }else{
		  index=0;
		 }
		}else{
		 rx_frame.data[index]=recive;
		 if(index==TransBuffSize-1){
		   index=0;
			 if((rx_frame.data[0]==0x69)&&(rx_frame.data[1]==0x7f)
				&&(rx_frame.data[TransBuffSize-2]==0x7f)
			  &&(rx_frame.data[TransBuffSize-1]==0x69)){
				  rx_frame.sta = rx_ok;
				}else{
				  rx_frame.sta = rx_fail;
				}
			 
		 }else
		 index++;
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
} 

void send_frame(void)
{
	static uint8_t i;
	uint16_t temp[8];
	tx_frame.data[0] = 0x69;
	tx_frame.data[1] = 0x89;
	tx_frame.data[2] = (uint8_t)((capture.roll.value&0xff00)>>8);
	tx_frame.data[3] = (uint8_t)((capture.roll.value&0x00ff));
	temp[0] = tx_frame.data[2]<<8|tx_frame.data[3];
	tx_frame.data[4] = (uint8_t)((capture.pitch.value&0xff00)>>8);
	tx_frame.data[5] = (uint8_t)((capture.pitch.value&0x00ff));
	temp[1] = tx_frame.data[4]<<8|tx_frame.data[5];
	tx_frame.data[6] = (uint8_t)((capture.throttle.value&0xff00)>>8);
	tx_frame.data[7] = (uint8_t)((capture.throttle.value&0x00ff));
	temp[2] = tx_frame.data[6]<<8|tx_frame.data[7];
	tx_frame.data[8] = (uint8_t)((capture.yaw.value&0xff00)>>8);
	tx_frame.data[9] = (uint8_t)((capture.yaw.value&0x00ff));
	temp[3] = tx_frame.data[8]<<8|tx_frame.data[9];
	tx_frame.data[10] = (uint8_t)((capture.aux1.value&0xff00)>>8);
	tx_frame.data[11] = (uint8_t)((capture.aux1.value&0x00ff));
	temp[4] = tx_frame.data[10]<<8|tx_frame.data[11];
	tx_frame.data[12] = (uint8_t)((capture.aux2.value&0xff00)>>8);
	tx_frame.data[13] = (uint8_t)((capture.aux2.value&0x00ff));
	temp[5] = tx_frame.data[12]<<8|tx_frame.data[13];
	tx_frame.data[14] = (uint8_t)((capture.aux3.value&0xff00)>>8);
	tx_frame.data[15] = (uint8_t)((capture.aux3.value&0x00ff));
	temp[6] = tx_frame.data[14]<<8|tx_frame.data[15];
	tx_frame.data[16] = (uint8_t)((capture.aux4.value&0xff00)>>8);
	tx_frame.data[17] = (uint8_t)((capture.aux4.value&0x00ff));
	temp[7] = tx_frame.data[16]<<8|tx_frame.data[17];
	tx_frame.data[18] = 0xff;
//	printf("%6d %6d %6d %6d %6d %6d %6d %6d \r\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7]);
	for(i=0;i<19;i++){
		USART3->DR = tx_frame.data[i];   
	  while((USART3->SR&0X40)==0){};//循环发送,直到发送完毕   
	}
}

