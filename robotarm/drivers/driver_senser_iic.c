#include "driver_senser_iic.h"
#include "delay.h"

void senser_iic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTC ��ʱ��
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = SENSER_IIC_SDA|SENSER_IIC_SCL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);  //���ݽṹ���ʼ��IIC��������
	
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA|SENSER_IIC_SCL);
}

//����IIC��ʼ�ź�
void iic_start(void)
{
	SENSER_SDA_OUT();     //sda�����
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA);	  	  
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);
	delay_us(2);
 	GPIO_ResetBits(GPIOC,SENSER_IIC_SDA);//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

//����IICֹͣ�ź�
void iic_stop(void)
{
	SENSER_SDA_OUT();//sda�����
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);
	GPIO_ResetBits(GPIOC,SENSER_IIC_SDA);//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);  
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA);//����I2C���߽����ź�
	delay_us(2);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t iic_wait_ack(void)
{
	uint8_t ucErrTime=0;
	SENSER_SDA_IN();      //SDA����Ϊ����  
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA);
	delay_us(2);	   
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);delay_us(2);	 
	while(SENSER_READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			iic_stop();
			return 1;
		}
	}
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);//ʱ�����0 	   
	return 0;  
} 

//����ACKӦ��
void iic_ack(void)
{
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);
	SENSER_SDA_OUT();
	GPIO_ResetBits(GPIOC,SENSER_IIC_SDA);
	delay_us(2);
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);
	delay_us(2);
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);
}

//������ACKӦ��		    
void iic_nack(void)
{
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);
	SENSER_SDA_OUT();
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA);
	delay_us(2);
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);
	delay_us(2);
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);
}					 				     

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void iic_send_byte(uint8_t txd)
{                        
    uint8_t t;   
	  SENSER_SDA_OUT(); 	    
    GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7){
				  GPIO_SetBits(GPIOC,SENSER_IIC_SDA);
				}else{
				  GPIO_ResetBits(GPIOC,SENSER_IIC_SDA);	
				}
        txd<<=1; 	  
				GPIO_SetBits(GPIOC,SENSER_IIC_SCL);
				delay_us(2); 
				GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);	
				delay_us(2);
    }	 
} 	    

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t iic_read_byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SENSER_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	 {
			GPIO_ResetBits(GPIOC,SENSER_IIC_SCL); 
			delay_us(2);
			GPIO_SetBits(GPIOC,SENSER_IIC_SCL);
			receive<<=1;
			if(SENSER_READ_SDA())receive++;   
		  delay_us(2); 
   }					 
    if (!ack)
      iic_nack();//����nACK
    else
      iic_ack(); //����ACK   
    return receive;
}

uint8_t senser_write_byte(uint8_t dev_addr,uint8_t reg_addr,uint8_t data)
{
  iic_start();
	iic_send_byte((dev_addr<<1)|0);
	if(iic_wait_ack())
	{
	  iic_stop();
		return 1;
	}
	iic_send_byte(reg_addr);
	iic_wait_ack();
	iic_send_byte(data);
	if(iic_wait_ack())
	{
	  iic_stop();
    return 1;
	}
	iic_stop();
	return 0;
}

uint8_t senser_read_byte(uint8_t dev_addr,uint8_t reg_addr)
{
  uint8_t data;
	iic_start();
	iic_send_byte((dev_addr<<1)|0);
	iic_wait_ack();
	iic_send_byte(reg_addr);
	iic_wait_ack();
	
	iic_start();
	iic_send_byte((dev_addr<<1)|1);
	iic_wait_ack();
	data = iic_read_byte(0);
	iic_stop();
	
	return data;
}

//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
uint8_t senser_write_len(uint8_t dev_addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
    iic_start(); 
	iic_send_byte((dev_addr<<1)|0);//����������ַ+д����	
	if(iic_wait_ack())	//�ȴ�Ӧ��
	{
		iic_stop();		 
		return 1;		
	}
    iic_send_byte(reg);	//д�Ĵ�����ַ
    iic_wait_ack();		//�ȴ�Ӧ��
	for(i=0;i<len;i++)
	{
		iic_send_byte(buf[i]);	//��������
		if(iic_wait_ack())		//�ȴ�ACK
		{
			iic_stop();	 
			return 1;		 
		}		
	}    
    iic_stop();	 
	return 0;	
} 
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
uint8_t senser_read_len(uint8_t dev_addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	iic_start(); 
	iic_send_byte((dev_addr<<1)|0);//����������ַ+д����	
	if(iic_wait_ack())	//�ȴ�Ӧ��
	{
		iic_stop();		 
		return 1;		
	}
    iic_send_byte(reg);	//д�Ĵ�����ַ
    iic_wait_ack();		//�ȴ�Ӧ��
    iic_start();
	iic_send_byte((dev_addr<<1)|1);//����������ַ+������	
    iic_wait_ack();		//�ȴ�Ӧ�� 
	while(len)
	{
		if(len==1)*buf=iic_read_byte(0);//������,����nACK 
		else *buf=iic_read_byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
    iic_stop();	//����һ��ֹͣ���� 
	return 0;	
}
