#include <math.h>
#include <stdio.h>

//typedef unsigned char uchar;
//typedef unsigned int uint;

///*���ֽ�дHMC5833*/
//void Single_Write_HMC5883(uchar Address,uchar Dat)
//{
//    IIC_Start();
//    HMC5883_Send_Byte(SlaveAddress);
//    HMC5883_Send_Byte(Address);
//    HMC5883_Send_Byte(Dat);
//    IIC_Stop();
//}

///*���ֽڶ�HMC5833*/
//void Multiple_Read_HMC5883(void)
//{
//    uchar i;  //��������HMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x3~0x5
//    IIC_Start();
//    HMC5883_Send_Byte(SlaveAddress);
//    HMC5883_Send_Byte(0x03);//���ʹ洢��Ԫ��ַ����0x03��ʼ 
//    IIC_Start();
//    HMC5883_Send_Byte(SlaveAddress+1);
//    for(i=0;i<6;i++) //������ȡ6����ַ���ݣ��洢��Rec_Data
//    {
//        Rec_Data[i]=HMC5883_Rec_Byte();
//        if(i==5)
//            IIC_SendAck(1); //���һ��������Ҫ��NOACK
//        else
//            IIC_SendAck(0); //��ӦACK
//    }
//    IIC_Stop();
//    Delay(100);
//}

////��ʼ��HMC5883��������Ҫ��ο�pdf�����޸�****
//void HMC5883_Init(void)
//{
//     Single_Write_HMC5883(0x02,0x00); 
//}
///*������*/
//void main(void)
//{
//    int X,Y,Z;
//    double Angle;
//    uint Acr;

//    LCD_Init();//LCD12232Һ����ʼ��
//    Dis_str(0x80,"3 ����������");
//    HMC5883_Init();//HMC5883��ʼ��

//    do
//    {
//        Multiple_Read_HMC5883();//�����������ݣ��洢��Rec_Data[]��
//        X=Rec_Data[0]<<8 | Rec_Data[1];//Combine MSB and LSB of X Data output register
//        Z=Rec_Data[2]<<8 | Rec_Data[3];//Combine MSB and LSB of Z Data output register
//        Y=Rec_Data[4]<<8 | Rec_Data[5];//Combine MSB and LSB of Y Data output register
//        Angle= atan2((double)Y,(double)X)*(180/3.14159265)+180;//��λ���Ƕ� (0~360)
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

