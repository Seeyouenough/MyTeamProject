#include "driver_senser_iic.h"
#include "delay.h"

void senser_iic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTC 的时钟
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = SENSER_IIC_SDA|SENSER_IIC_SCL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);  //根据结构体初始化IIC引脚配置
	
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA|SENSER_IIC_SCL);
}

//产生IIC起始信号
void iic_start(void)
{
	SENSER_SDA_OUT();     //sda线输出
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA);	  	  
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);
	delay_us(2);
 	GPIO_ResetBits(GPIOC,SENSER_IIC_SDA);//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);//钳住I2C总线，准备发送或接收数据 
}	  

//产生IIC停止信号
void iic_stop(void)
{
	SENSER_SDA_OUT();//sda线输出
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);
	GPIO_ResetBits(GPIOC,SENSER_IIC_SDA);//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	GPIO_SetBits(GPIOC,SENSER_IIC_SCL);  
	GPIO_SetBits(GPIOC,SENSER_IIC_SDA);//发送I2C总线结束信号
	delay_us(2);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t iic_wait_ack(void)
{
	uint8_t ucErrTime=0;
	SENSER_SDA_IN();      //SDA设置为输入  
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
	GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);//时钟输出0 	   
	return 0;  
} 

//产生ACK应答
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

//不产生ACK应答		    
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

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void iic_send_byte(uint8_t txd)
{                        
    uint8_t t;   
	  SENSER_SDA_OUT(); 	    
    GPIO_ResetBits(GPIOC,SENSER_IIC_SCL);//拉低时钟开始数据传输
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

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t iic_read_byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SENSER_SDA_IN();//SDA设置为输入
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
      iic_nack();//发送nACK
    else
      iic_ack(); //发送ACK   
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

//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
uint8_t senser_write_len(uint8_t dev_addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
    iic_start(); 
	iic_send_byte((dev_addr<<1)|0);//发送器件地址+写命令	
	if(iic_wait_ack())	//等待应答
	{
		iic_stop();		 
		return 1;		
	}
    iic_send_byte(reg);	//写寄存器地址
    iic_wait_ack();		//等待应答
	for(i=0;i<len;i++)
	{
		iic_send_byte(buf[i]);	//发送数据
		if(iic_wait_ack())		//等待ACK
		{
			iic_stop();	 
			return 1;		 
		}		
	}    
    iic_stop();	 
	return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
uint8_t senser_read_len(uint8_t dev_addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	iic_start(); 
	iic_send_byte((dev_addr<<1)|0);//发送器件地址+写命令	
	if(iic_wait_ack())	//等待应答
	{
		iic_stop();		 
		return 1;		
	}
    iic_send_byte(reg);	//写寄存器地址
    iic_wait_ack();		//等待应答
    iic_start();
	iic_send_byte((dev_addr<<1)|1);//发送器件地址+读命令	
    iic_wait_ack();		//等待应答 
	while(len)
	{
		if(len==1)*buf=iic_read_byte(0);//读数据,发送nACK 
		else *buf=iic_read_byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    iic_stop();	//产生一个停止条件 
	return 0;	
}
