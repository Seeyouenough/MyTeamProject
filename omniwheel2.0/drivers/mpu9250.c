#include "mpu9250.h"
#include "mpuiic.h"
#include "delay.h"
#include "stdio.h"

/* ���������飺����ֵ,ƫ��ֵ,�������ֵ */
ak8975_t ak8975 = { {0,0,0},{-1,-1,-1},{0,0,0} };
/* ������Ӳ������ */
u8 hard_error_ak8975;
/* ������У׼��ʶ */
bool Mag_CALIBRATED=false;



/* У׼ʱ�ļ������ */
#define OFFSET_AV_NUM 	    50
/* ���������˲���ֵ���� */
#define FILTER_NUM 			10
/* Gyro���ٶȵĵ�λת�� */
#define TO_ANGLE 			0.06103f

/* MPU6050�ṹ�� */
MPU6050_STRUCT mpu6050;
/* ������У׼�����ۼӺ� */
s32 sum_temp[7]= {0,0,0,0,0,0,0};
/* ������У׼��ʱ���� */
u16 acc_sum_cnt = 0,gyro_sum_cnt = 0;
/* �˲������������� */
s16 FILT_BUF[ITEMS][(FILTER_NUM + 1)];
/* �˲��������������±� */
uint8_t filter_cnt = 0,filter_cnt_old = 0;
/* �ó�У׼�����ʱ���� */
float mpu6050_tmp[ITEMS];


/*----------------------------------------------------------
 + ʵ�ֹ��ܣ�MPU6050���ٶȼơ�����������У׼
----------------------------------------------------------*/
void MPU6050_Data_Offset()
{
    /* ���ٶȼ�У׼ */
    if(mpu6050.Acc_CALIBRATE == 1)
    {
        /* �������ۼ� */
        acc_sum_cnt++;
        sum_temp[A_X] += mpu6050.Acc_I16.x;
        sum_temp[A_Y] += mpu6050.Acc_I16.y;
        sum_temp[A_Z] += mpu6050.Acc_I16.z - 4096;
        sum_temp[TEM] += mpu6050.Tempreature;

        /* �жϼ����������� */
        if( acc_sum_cnt >= OFFSET_AV_NUM )
        {
            /* ����У������ */
            mpu6050.Acc_Offset.x = sum_temp[A_X]/OFFSET_AV_NUM;
            mpu6050.Acc_Offset.y = sum_temp[A_Y]/OFFSET_AV_NUM;
            mpu6050.Acc_Offset.z = sum_temp[A_Z]/OFFSET_AV_NUM;
            mpu6050.Acc_Temprea_Offset = sum_temp[TEM]/OFFSET_AV_NUM;
            /* ������̱��� */
            acc_sum_cnt =0;
            mpu6050.Acc_CALIBRATE = 0;
            sum_temp[A_X] = sum_temp[A_Y] = sum_temp[A_Z] = sum_temp[TEM] = 0;
        }
    }

    /* ������У׼ */
    if(mpu6050.Gyro_CALIBRATE)
    {
        /* �������ۼ� */
        gyro_sum_cnt++;
        sum_temp[G_X] += mpu6050.Gyro_I16.x;
        sum_temp[G_Y] += mpu6050.Gyro_I16.y;
        sum_temp[G_Z] += mpu6050.Gyro_I16.z;
        sum_temp[TEM] += mpu6050.Tempreature;

        /* �жϼ����������� */
        if( gyro_sum_cnt >= OFFSET_AV_NUM )
        {
            /* ����У������ */
            mpu6050.Gyro_Offset.x = (float)sum_temp[G_X]/OFFSET_AV_NUM;
            mpu6050.Gyro_Offset.y = (float)sum_temp[G_Y]/OFFSET_AV_NUM;
            mpu6050.Gyro_Offset.z = (float)sum_temp[G_Z]/OFFSET_AV_NUM;
            mpu6050.Gyro_Temprea_Offset = sum_temp[TEM]/OFFSET_AV_NUM;
            /* ������̱��� */
            gyro_sum_cnt =0;
            mpu6050.Gyro_CALIBRATE = 0;
            sum_temp[G_X] = sum_temp[G_Y] = sum_temp[G_Z] = sum_temp[TEM] = 0;
        }
    }
}

/*----------------------------------------------------------
 + ʵ�ֹ��ܣ���������ȵ�������2ms
----------------------------------------------------------*/
void Call_MPU6050_Data_Prepare(float T)
{
	  u8  i;
    /* �˲�����������ʱ���� */
    s32 FILT_TMP[ITEMS] = {0,0,0,0,0,0,0};
    /* ԭʼ���ݶ�ȡ��ʱ���� */
    u8 mpu6050_buffer[14];
    /* ������ԭʼ������ʱ���� */
    float Gyro_tmp[3];

    /* �ж��Ƿ���MPU6050���ٶȼơ�����������У׼ */
    MPU6050_Data_Offset();

		MPU_Get_Gyroscope(&mpu6050.Gyro_I16.x,&mpu6050.Gyro_I16.y,&mpu6050.Gyro_I16.z);
		MPU_Get_Accelerometer(&mpu6050.Acc_I16.x,&mpu6050.Acc_I16.y,&mpu6050.Acc_I16.z);

    /* ������ԭʼ������ʱ���� */
    Gyro_tmp[0] = mpu6050.Gyro_I16.x ;
    Gyro_tmp[1] = mpu6050.Gyro_I16.y ;
    Gyro_tmp[2] = mpu6050.Gyro_I16.z ;

    /* ��ȡ�¶���Ϣ���˲� */
    mpu6050.Tempreature = ((((int16_t)mpu6050_buffer[6]) << 8) | mpu6050_buffer[7]);
    mpu6050.TEM_LPF += 2 *3.14f *T *(mpu6050.Tempreature - mpu6050.TEM_LPF);
    mpu6050.Ftempreature = mpu6050.TEM_LPF/340.0f + 36.5f;

    /* �ó�У׼�����ʱ���� */
    mpu6050_tmp[A_X] = (mpu6050.Acc_I16.x - mpu6050.Acc_Offset.x) ;
    mpu6050_tmp[A_Y] = (mpu6050.Acc_I16.y - mpu6050.Acc_Offset.y) ;
    mpu6050_tmp[A_Z] = (mpu6050.Acc_I16.z - mpu6050.Acc_Offset.z) ;
    mpu6050_tmp[G_X] = Gyro_tmp[0] - mpu6050.Gyro_Offset.x ;
    mpu6050_tmp[G_Y] = Gyro_tmp[1] - mpu6050.Gyro_Offset.y ;
    mpu6050_tmp[G_Z] = Gyro_tmp[2] - mpu6050.Gyro_Offset.z ;

    /* ���������˲������±���λ */
    if( ++filter_cnt > FILTER_NUM )
    {
        filter_cnt = 0;
        filter_cnt_old = 1;
    }
    else
        filter_cnt_old = (filter_cnt == FILTER_NUM)? 0 : (filter_cnt + 1);

    /* �����˲�����������ʱ���� */
    FILT_BUF[A_X][filter_cnt] = mpu6050_tmp[A_X];
    FILT_BUF[A_Y][filter_cnt] = mpu6050_tmp[A_Y];
    FILT_BUF[A_Z][filter_cnt] = mpu6050_tmp[A_Z];
    FILT_BUF[G_X][filter_cnt] = mpu6050_tmp[G_X];
    FILT_BUF[G_Y][filter_cnt] = mpu6050_tmp[G_Y];
    FILT_BUF[G_Z][filter_cnt] = mpu6050_tmp[G_Z];
    /* �����˲������������� */
    for(i=0; i<FILTER_NUM; i++)
    {
        FILT_TMP[A_X] += FILT_BUF[A_X][i];
        FILT_TMP[A_Y] += FILT_BUF[A_Y][i];
        FILT_TMP[A_Z] += FILT_BUF[A_Z][i];
        FILT_TMP[G_X] += FILT_BUF[G_X][i];
        FILT_TMP[G_Y] += FILT_BUF[G_Y][i];
        FILT_TMP[G_Z] += FILT_BUF[G_Z][i];
    }

    /* �ó����������� */
    mpu6050.Acc.x  = (float)( FILT_TMP[A_X] )/(float)FILTER_NUM;
    mpu6050.Acc.y  = (float)( FILT_TMP[A_Y] )/(float)FILTER_NUM;
    mpu6050.Acc.z  = (float)( FILT_TMP[A_Z] )/(float)FILTER_NUM;
    mpu6050.Gyro.x  = (float)( FILT_TMP[G_X] )/(float)FILTER_NUM;
    mpu6050.Gyro.y  = (float)( FILT_TMP[G_Y] )/(float)FILTER_NUM;
    mpu6050.Gyro.z  = (float)( FILT_TMP[G_Z] )/(float)FILTER_NUM;

    /* �����ݻ��㵽���ٶ� */
    mpu6050.Gyro_deg.x = mpu6050.Gyro.x *TO_ANGLE;
    mpu6050.Gyro_deg.y = mpu6050.Gyro.y *TO_ANGLE;
    mpu6050.Gyro_deg.z = mpu6050.Gyro.z *TO_ANGLE;
}

/*----------------------------------------------------------
 + ʵ�ֹ��ܣ��ж��Ƿ������У׼
----------------------------------------------------------*/
void AK8975_Set_CalOffset(void)
{
    /* У׼ǰ��ȱʡֵ */
    static xyz_f_t	MagMAX = { -100 , -100 , -100 }, MagMIN = { 100 , 100 , 100 };
    /* У׼ʱ����� */
    static uint16_t cnt_m=0;

    /* �ж��Ƿ������У׼ */
    if(Mag_CALIBRATED)
    {
        /* У׼ʱ����� */
        cnt_m++;
        /* ���浥һ������ֵ */
        if(ABS(ak8975.Mag_Adc.x)<400)
        {
            MagMAX.x = MAX(ak8975.Mag_Adc.x, MagMAX.x);
            MagMIN.x = MIN(ak8975.Mag_Adc.x, MagMIN.x);
        }
        /* ���浥һ������ֵ */
        if(ABS(ak8975.Mag_Adc.y)<400)
        {
            MagMAX.y = MAX(ak8975.Mag_Adc.y, MagMAX.y);
            MagMIN.y = MIN(ak8975.Mag_Adc.y, MagMIN.y);
        }
        /* ���浥һ������ֵ */
        if(ABS(ak8975.Mag_Adc.z)<400)
        {
            MagMAX.z = MAX(ak8975.Mag_Adc.z, MagMAX.z);
            MagMIN.z = MIN(ak8975.Mag_Adc.z, MagMIN.z);
        }
        /* У׼ʱ�䵽 */
        if(cnt_m == CALIBRATING_MAG_CYCLES)
        {
            /* ����У׼������� */
            ak8975.Mag_Offset.x = (MagMAX.x + MagMIN.x)/2;
            ak8975.Mag_Offset.y = (MagMAX.y + MagMIN.y)/2;
            ak8975.Mag_Offset.z = (MagMAX.z + MagMIN.z)/2;
            /* У׼��У׼ʱ������ */
            cnt_m = 0;
            Mag_CALIBRATED = false;
        }
    }
}

/*----------------------------------------------------------
 + ʵ�ֹ��ܣ���������ȵ�������10ms
----------------------------------------------------------*/
void Call_AK8975(void)
{
    /* �˲�����������ʱ���� */
    static xyz_f_t Mag_Adc_fill[MAG_FILTER_NUM];
    /* �˲����������ܺ� */
    static xyz_f_t Mag_Adc_fill_total;
    /* �˲��������������±� */
    static char temp_i;
    short temp[3];
	  MPU_Get_Magnetometer(&temp[0],&temp[1],&temp[2]);
    /* �����Ʋ���ԭʼ���� */
    ak8975.Mag_Adc.x =   temp[0];
    ak8975.Mag_Adc.y =   temp[1];
    ak8975.Mag_Adc.z =   temp[2];

    /* �����˲�����������ʱ���� */
    Mag_Adc_fill[temp_i].x = ak8975.Mag_Adc.x;
    Mag_Adc_fill[temp_i].y = ak8975.Mag_Adc.y;
    Mag_Adc_fill[temp_i].z = ak8975.Mag_Adc.z;
    /* ���������˲������±���λ */
    temp_i++;
    if(temp_i>=MAG_FILTER_NUM)temp_i=0;

    /* ���»��������˲������ܺ� */
    Mag_Adc_fill_total.x+=ak8975.Mag_Adc.x;
    Mag_Adc_fill_total.y+=ak8975.Mag_Adc.y;
    Mag_Adc_fill_total.z+=ak8975.Mag_Adc.z;
    Mag_Adc_fill_total.x-=Mag_Adc_fill[temp_i].x;
    Mag_Adc_fill_total.y-=Mag_Adc_fill[temp_i].y;
    Mag_Adc_fill_total.z-=Mag_Adc_fill[temp_i].z;

    /* �ó����������� */
    ak8975.Mag_Val.x = Mag_Adc_fill_total.x / MAG_FILTER_NUM;
    ak8975.Mag_Val.y = Mag_Adc_fill_total.y / MAG_FILTER_NUM;
    ak8975.Mag_Val.z = Mag_Adc_fill_total.z / MAG_FILTER_NUM;

    /* �ж��Ƿ������У׼ */
    AK8975_Set_CalOffset();
}


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//MPU9250��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//��ʼ��MPU9250
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU9250_Init(void)
{
    u8 res=0;
    IIC_Init();     //��ʼ��IIC����
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//��λMPU9250
    delay_ms(100);  //��ʱ100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//����MPU9250
    MPU_Set_Gyro_Fsr(3);					        	//�����Ǵ�����,��2000dps
	  MPU_Set_Accel_Fsr(2);					       	 	//���ٶȴ�����,��8g
    MPU_Set_Rate(1000);						       	 	//���ò�����1000Hz
    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //�ر������ж�
	  MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C��ģʽ�ر�
	  MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
    if(res==MPU6500_ID) //����ID��ȷ
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
		    MPU_Set_Rate(1000);						       	//���ò�����Ϊ1000Hz   
    }else{
			printf("err ID:%d\r\n",res);
		  return 1;
		}
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//��ȡAK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//����AK8963Ϊ���β���ģʽ
    }else{
			printf("err mag ID:%d\r\n",res);
		  return 2;
		}
    return 0;
}

//����MPU9250�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU9250���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

//����MPU9250�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}

//����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(42);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=21+((double)raw)/333.87;  
    return temp*100;;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res; 
	res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}


//�õ�������ֵ(ԭʼֵ)
//mx,my,mz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
  u8 buf[6],res;  
	res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{
		*mx=((u16)buf[1]<<8)|buf[0];  
		*my=((u16)buf[3]<<8)|buf[2];  
		*mz=((u16)buf[5]<<8)|buf[4];
	}
	MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
	return res;;
}

//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    for(i=0;i<len;i++)
    {
        IIC_Send_Byte(buf[i]);  //��������
        if(IIC_Wait_Ack())      //�ȴ�ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
} 

//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
	IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //����������ַ+������
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//������,����nACK 
		else *buf=IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++;  
    }
    IIC_Stop();                 //����һ��ֹͣ����
    return 0;       
}

//IICдһ���ֽ� 
//devaddr:����IIC��ַ
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    IIC_Send_Byte(data);        //��������
    if(IIC_Wait_Ack())          //�ȴ�ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
	  IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //����������ַ+������
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    res=IIC_Read_Byte(0);		//������,����nACK  
    IIC_Stop();                 //����һ��ֹͣ����
    return res;  
}
