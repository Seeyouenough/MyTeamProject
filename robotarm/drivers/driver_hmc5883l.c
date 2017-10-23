#include <math.h>
#include <stdio.h>

//typedef unsigned char uchar;
//typedef unsigned int uint;

///*单字节写HMC5833*/
//void Single_Write_HMC5883(uchar Address,uchar Dat)
//{
//    IIC_Start();
//    HMC5883_Send_Byte(SlaveAddress);
//    HMC5883_Send_Byte(Address);
//    HMC5883_Send_Byte(Dat);
//    IIC_Stop();
//}

///*多字节读HMC5833*/
//void Multiple_Read_HMC5883(void)
//{
//    uchar i;  //连续读出HMC5883内部角度数据，地址范围0x3~0x5
//    IIC_Start();
//    HMC5883_Send_Byte(SlaveAddress);
//    HMC5883_Send_Byte(0x03);//发送存储单元地址，从0x03开始 
//    IIC_Start();
//    HMC5883_Send_Byte(SlaveAddress+1);
//    for(i=0;i<6;i++) //连续读取6个地址数据，存储在Rec_Data
//    {
//        Rec_Data[i]=HMC5883_Rec_Byte();
//        if(i==5)
//            IIC_SendAck(1); //最后一个数据需要回NOACK
//        else
//            IIC_SendAck(0); //回应ACK
//    }
//    IIC_Stop();
//    Delay(100);
//}

////初始化HMC5883，根据需要请参考pdf进行修改****
//void HMC5883_Init(void)
//{
//     Single_Write_HMC5883(0x02,0x00); 
//}
///*主函数*/
//void main(void)
//{
//    int X,Y,Z;
//    double Angle;
//    uint Acr;

//    LCD_Init();//LCD12232液晶初始化
//    Dis_str(0x80,"3 轴数字罗盘");
//    HMC5883_Init();//HMC5883初始化

//    do
//    {
//        Multiple_Read_HMC5883();//连续读出数据，存储在Rec_Data[]中
//        X=Rec_Data[0]<<8 | Rec_Data[1];//Combine MSB and LSB of X Data output register
//        Z=Rec_Data[2]<<8 | Rec_Data[3];//Combine MSB and LSB of Z Data output register
//        Y=Rec_Data[4]<<8 | Rec_Data[5];//Combine MSB and LSB of Y Data output register
//        Angle= atan2((double)Y,(double)X)*(180/3.14159265)+180;//单位：角度 (0~360)
//        Angle*=10;
//        Acr=(uint)Angle;

//        Send_DATA(0x92,0);
//        Send_DATA(Acr%10000/1000+0x30,1);
//        Send_DATA(Acr%1000/100+0x30,1);
//        Send_DATA(Acr%100/10+0x30,1);
//        Send_DATA('.',1);
//        Send_DATA(Acr%10+0x30,1);
//    }
//    while(1);
//}

