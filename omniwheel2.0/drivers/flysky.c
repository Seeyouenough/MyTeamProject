#include "led.h"
#include "delay.h"
#include "usart.h"
#include "flysky.h"
#include "stmflash.h"
#include "timer.h"


#define WZ12Mhz		1			// 0	1     0	    0	//
#define WZ16Mhz		0			// 0	0	  1	    0	//
#define WZ24Mhz		0			// 0	0	  0	    1	//

#define FLASH_SAVE_ADDR  0X08020000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

//定义指示灯
#define LED LED1

//定义对码线
#define bind PAin(14)


//定义无线模块的管脚
#define GIO PBin(0)  //只输入
#define MDI PBout(1)  //双模式
#define SCK PBout(10)  //只输出
#define CSN PBout(11)  //只输出

//IO方向设置
#define MDI_IN()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=8<<4;}
#define MDI_OUT() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=3<<4;}
#define MDI_READ() PBin(1)

u8 TR0=0;
#define EA_ON()  {TIM_ITConfig(  TIM6,TIM_IT_Update ,ENABLE ); TR0=1;}
#define EA_OFF() {TIM_ITConfig(  TIM6,TIM_IT_Update ,DISABLE );TR0=0;}
const u8  A7105_regs[]={			//模块寄存器设定表
    0xff, 0x42, 0x00, 0x14, 0x00, 0xff, 0xff ,0x00, 0x00, 0x00, 0x00, 0x01, 0x21, 0x05, 0x00, 0x50,
    0x9e, 0x4b, 0x00, 0x02, 0x16, 0x2b, 0x12, 0x00, 0x62, 0x80, 0x80, 0x00, 0x0a, 0x32, 0xc3, 0x0f,
    0x13, 0xc3, 0x00, 0xff, 0x00, 0x00, 0x3b, 0x00, 0x17, 0x47, 0x80, 0x03, 0x01, 0x45, 0x18, 0x00,
    0x01, 0x0f, 0xff,
};
const u8  tx_channels[16][16]={		//跳频表
  {0x0a, 0x5a, 0x14, 0x64, 0x1e, 0x6e, 0x28, 0x78, 0x32, 0x82, 0x3c, 0x8c, 0x46, 0x96, 0x50, 0xa0},
  {0xa0, 0x50, 0x96, 0x46, 0x8c, 0x3c, 0x82, 0x32, 0x78, 0x28, 0x6e, 0x1e, 0x64, 0x14, 0x5a, 0x0a},
  {0x0a, 0x5a, 0x50, 0xa0, 0x14, 0x64, 0x46, 0x96, 0x1e, 0x6e, 0x3c, 0x8c, 0x28, 0x78, 0x32, 0x82},
  {0x82, 0x32, 0x78, 0x28, 0x8c, 0x3c, 0x6e, 0x1e, 0x96, 0x46, 0x64, 0x14, 0xa0, 0x50, 0x5a, 0x0a},
  {0x28, 0x78, 0x0a, 0x5a, 0x50, 0xa0, 0x14, 0x64, 0x1e, 0x6e, 0x3c, 0x8c, 0x32, 0x82, 0x46, 0x96},
  {0x96, 0x46, 0x82, 0x32, 0x8c, 0x3c, 0x6e, 0x1e, 0x64, 0x14, 0xa0, 0x50, 0x5a, 0x0a, 0x78, 0x28},
  {0x50, 0xa0, 0x28, 0x78, 0x0a, 0x5a, 0x1e, 0x6e, 0x3c, 0x8c, 0x32, 0x82, 0x46, 0x96, 0x14, 0x64},
  {0x64, 0x14, 0x96, 0x46, 0x82, 0x32, 0x8c, 0x3c, 0x6e, 0x1e, 0x5a, 0x0a, 0x78, 0x28, 0xa0, 0x50},
  {0x50, 0xa0, 0x46, 0x96, 0x3c, 0x8c, 0x28, 0x78, 0x0a, 0x5a, 0x32, 0x82, 0x1e, 0x6e, 0x14, 0x64},
  {0x64, 0x14, 0x6e, 0x1e, 0x82, 0x32, 0x5a, 0x0a, 0x78, 0x28, 0x8c, 0x3c, 0x96, 0x46, 0xa0, 0x50},
  {0x46, 0x96, 0x3c, 0x8c, 0x50, 0xa0, 0x28, 0x78, 0x0a, 0x5a, 0x1e, 0x6e, 0x32, 0x82, 0x14, 0x64},
  {0x64, 0x14, 0x82, 0x32, 0x6e, 0x1e, 0x5a, 0x0a, 0x78, 0x28, 0xa0, 0x50, 0x8c, 0x3c, 0x96, 0x46},
  {0x46, 0x96, 0x0a, 0x5a, 0x3c, 0x8c, 0x14, 0x64, 0x50, 0xa0, 0x28, 0x78, 0x1e, 0x6e, 0x32, 0x82},
  {0x82, 0x32, 0x6e, 0x1e, 0x78, 0x28, 0xa0, 0x50, 0x64, 0x14, 0x8c, 0x3c, 0x5a, 0x0a, 0x96, 0x46},
  {0x46, 0x96, 0x0a, 0x5a, 0x50, 0xa0, 0x3c, 0x8c, 0x28, 0x78, 0x1e, 0x6e, 0x32, 0x82, 0x14, 0x64},
  {0x64, 0x14, 0x82, 0x32, 0x6e, 0x1e, 0x78, 0x28, 0x8c, 0x3c, 0xa0, 0x50, 0x5a, 0x0a, 0x96, 0x46},
};
u32 id;								//对码ID
u8 chanrow,chancol,chanoffset,channel;
u8 txid[21]={0,0,0,0,0,220,5,220,5,232,3,220,5,220,5,220,5,220,5,220,5};
u8 packet[21];
u8 Htemp[8]={5,5,3,5,5,5,5,5};
u8 Ltemp[8]={220,220,232,220,220,220,220,220};
u8 ch=0;
u16 mss;
u8 hch,cb;
u16 CH[8];

void RC_GPIO_Init()
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //MDI 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB12
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 //BIND 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA11
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;				 //SCK  端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB12
}


void flysky_timer_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 19999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =71; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM6, //TIM6
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM6, ENABLE);  //使能TIMx外设
}

void TIM6_IRQHandler(void)   //TIM6中断
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
			  hch=1;
	      mss++;
		}
}

u8 EEPROM_read(u8 addr){
    unsigned short data;                       	//Data buffer
    STMFLASH_Read(FLASH_SAVE_ADDR+addr*2,&data,1);
    return data&0x00FF;                     //Return Flash data
}
void EEPROM_write(u8 addr,u8 dat){
	  unsigned short data = dat;
    STMFLASH_Write(FLASH_SAVE_ADDR+addr*2,&data,1);
    delay_us(1);                        //MCU will hold here until ISP/IAP/EEPROM operation complete
}
void EEPROM_clean(u8 addr){
    EEPROM_write(addr,0x00);
    delay_us(1);                        //MCU will hold here until ISP/IAP/EEPROM operation complete
}
void IDWR(u8 wr){ 					// 1:保存	0:读取
	u8 i;
	if(wr)EEPROM_clean(0),delay_ms(10);
	for(i=0;i<21;i++){
		if(wr){
			EEPROM_write(i,txid[i]); }
		//	txid[i]=packet[i];}
		else txid[i]=EEPROM_read(i);
	}
}
void SPIwrite(u8 command){   		//时序写
	u8 n=8; 
	MDI_OUT();
	SCK=0;MDI=0;
	while(n--) {
		if(command&0x80)MDI=1;
		else MDI=0;
		SCK=1;delay_us(1);SCK=0;
		command = command << 1;
	}
	MDI=1;
}  
u8 SPIread(void){					//时序读
	u8 i,result=0;
	for(i=0;i<8;i++) {
    MDI_IN();		
		if(MDI_READ()==1)result=(result<<1)|0x01;
		else result=result<<1;
		SCK=1;delay_us(1);SCK=0;delay_us(1);
	}return result;
}
void AREG_write(u8 address, u8 dat){//写寄存器
	CSN=0;
	SPIwrite(address); 
	delay_us(1);
	SPIwrite(dat);  
	CSN=1;
} 
u8 AREG_read(u8 address){ 			//读寄存器
	u8 result;
	CSN=0;
	address |=0x40;
	SPIwrite(address);
	result = SPIread();  
	CSN=1;
	return(result); 
} 
void A7105_WriteID(u32 ida) {		//写模块ID
	CSN=0;
	SPIwrite(0x06);
	SPIwrite((ida>>24)&0xff); 
	SPIwrite((ida>>16)&0xff);
	SPIwrite((ida>>8)&0xff);
	SPIwrite((ida>>0)&0xff);
	CSN=1;
}
void Read_Packet(){					//读数据包
	u8 i;
	CSN=0;
	SPIwrite(0x45);
	for (i=0;i<21;i++)packet[i]=SPIread();
	CSN=1;
}
void Strobe(u8 address){			//模块设定
	CSN=0;
	SPIwrite(address);
	CSN=1;
}
void bind_Flysky(){					//对频
	u8 i,counter1=255;
	Strobe(0xA0);
	Strobe(0xF0);
	AREG_write(0x0F,0x00);//比发射低一频道
	Strobe(0xC0);
	while(counter1){
		printf("step-4\r\n");
		delay_ms(50);
		if(counter1&0x04)LED=0;
		else LED=1;		
		if(GIO==0){
			printf("step-5\r\n");
			if((AREG_read(0x00)&0x60)==0){
        printf("step-6\r\n");				
				Read_Packet();
			//	IDWR(1);//保存ID
				for(i=0;i<21;i++)txid[i]=packet[i];
				break;
			}
			else{
				printf("step-7\r\n");
				Strobe(0xA0);
				Strobe(0xF0);
				AREG_write(0x0F,0x00);
				Strobe(0xC0);
				continue;
			}
		}
		else{
		printf("step-8\r\n");
		--counter1;
		if (counter1==0)counter1=255;
		}
	}	
}
void flysky_cb(){					//主循环
	u8 i,x;
	channel=tx_channels[chanrow][chancol]-chanoffset;//跳频道
	channel-=1;
	Strobe(0xA0);
	Strobe(0xF0);
	AREG_write(0x0F,channel);//设定频道
	Strobe(0xC0);
	chancol=(chancol+1)%16;
	while(1){
		if(mss>800){    //无信号1.5s后失控保护
		#ifdef GUDING
			Ltemp[2]=232;Htemp[2]=3;
		#endif	
			for(i=0;i<8;i++){
		#ifdef GUDING	
				if(i==2)continue;
				Ltemp[i]=220;
				Htemp[i]=5;
		#else		
				Ltemp[i]=txid[2*i+5];
				Htemp[i]=txid[2*i+6];
		#endif
			}
		} 
		 if(hch){ 		//大于2ms加跳
			hch=0;LED=1;
			chancol=(chancol+1)%16;
		//	channel=tx_channels[chanrow][chancol]-1-chanoffset;
			break;
		}
		if (GIO==1)continue;
		x=AREG_read(0x00);
		if(x&0x60)continue;//数据检验
		Read_Packet();//读遥控数据  	     ↓↓↓↓↓匹配ID
		if (!(packet[1]==txid[1])&&!(packet[2]==txid[2])&&!(packet[3]==txid[3])&&!(packet[4]==txid[4]))continue;
		mss=0;
		if(cb){
			for (i=0;i<8;i++){//输出		1000~2000
				EA_OFF();
				x=packet[6+(2*i)];
				if((x>=3)&&(x<=8) ){	//防止溢出
					Ltemp[i]=packet[5+(2*i)];
					Htemp[i]=x;
				}
				EA_ON();
			}
		}
		if(cb)LED=0;
		break;
	}
}
void flysky_init(){					//初始化
	u8 i,k;
	u16 HT;
	flysky_timer_init();
	RC_GPIO_Init();
  EA_ON();
	MDI_OUT();
	bind=1;CSN=1;MDI=1;SCK=0;
	delay_ms(10);
	AREG_write(0x00,0x00);
	A7105_WriteID(0x5475c52A);
	for (i = 0; i < 0x33; i++){
		if(A7105_regs[i] != 0xff)
			AREG_write(i, A7105_regs[i]);
	}

	Strobe(0xA0);
	AREG_write(0x02,0x01);
	while(AREG_read(0x02));
	AREG_write(0x24,0x13);
	AREG_write(0x26,0x3b);
	AREG_write(0x0F,0x00);
	AREG_write(0x02,0x02);
	while(AREG_read(0x02));
	AREG_write(0x0F,0xA0);
	AREG_write(0x02,0x02);
	while(AREG_read(0x02));
	AREG_write(0x25,0x08);
	Strobe(0xA0);
  printf("step-1\r\n");
 	while(1){
		LED=0;
		delay_ms(500);
		LED=1;
		delay_ms(500);
		printf("step-2\r\n");
		if(bind==1){
			bind_Flysky();//对频
			printf("step-9\r\n");
			LED=0;	
			delay_ms(500);
			LED=1;
			delay_ms(500);
			EA_ON();
			cb=0;
			break;
		}
		else{
			IDWR(0);		//读取ID
			printf("step-3\r\n");
			cb=1;
			if(txid[0]==0xaa)break;
			else continue;
		}
	}			//↓↓↓↓↓↓↓↓合拼ID 
	id=(txid[1]|((u32)txid[2]<<8)|((u32)txid[3]<<16)|((u32)txid[4]<<24));	
	chanrow=id%16;
	chanoffset=(id&0xff)/16;
	chancol=0;
	if(chanoffset>9)chanoffset=9;
	printf("step-10\r\n");
	while(TR0){
		printf("step-11\r\n");
		if(!cb){IDWR(1);LED=1;EA_OFF();while(1){LED=0;delay_ms(1000);LED=1;delay_ms(1500);};}
		flysky_cb();//4:1024  5:1280  6:1536  7:1792  8:2048
		EA_OFF();
		if(mss>5){mss=0;
			for(i=0;i<8;i++){
				if((i>3)||(i==2)){
					Ltemp[i]=txid[2*i+5]=packet[2*i+5];
					Htemp[i]=txid[2*i+6]=packet[2*i+6];
					continue;
				}
				if((packet[2*i+6]<5)||(packet[2*i+6]>6)){
					if(k&(1<<i)){k&=(0xfe<<i);LED=0;
						HT=(u16)txid[2*i+5]|(u16)(txid[2*i+6]<<8);
						if(packet[2*i+6]<5){if(HT>900)HT-=100;}
						else if(HT<2000)HT+=100;
						Ltemp[i]=txid[2*i+5]=HT&0xff;
						Htemp[i]=txid[2*i+6]=(HT>>8)&0xff;}
					}
				else k|=(1<<i);			
			}			
		}
	}
}
void RC_Init(void){						
	flysky_init();
	EA_ON();	//跳频定时器
}

void CH_OUT(){				//输出定时器
	CH[ch]=(Htemp[ch]<<8)|Ltemp[ch];
//	printf("%6d%6d%6d\r\n",CH[2],CH[3],CH[4]);
	if(ch < 7)ch++;//8通道循环
	else ch=0;
}

void flysky_loop()
{
	flysky_cb();
	CH_OUT();
}

