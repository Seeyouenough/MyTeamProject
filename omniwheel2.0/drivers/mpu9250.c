#include "mpu9250.h"
#include "mpuiic.h"
#include "delay.h"
#include "stdio.h"

/* 磁力计数组：采样值,偏移值,纠正后的值 */
ak8975_t ak8975 = { {0,0,0},{-1,-1,-1},{0,0,0} };
/* 磁力计硬件故障 */
u8 hard_error_ak8975;
/* 磁力计校准标识 */
bool Mag_CALIBRATED=false;



/* 校准时的计算次数 */
#define OFFSET_AV_NUM 	    50
/* 滑动窗口滤波数值个数 */
#define FILTER_NUM 			10
/* Gyro角速度的单位转换 */
#define TO_ANGLE 			0.06103f

/* MPU6050结构体 */
MPU6050_STRUCT mpu6050;
/* 传感器校准数据累加和 */
s32 sum_temp[7]= {0,0,0,0,0,0,0};
/* 传感器校准临时计数 */
u16 acc_sum_cnt = 0,gyro_sum_cnt = 0;
/* 滤波滑动窗口数组 */
s16 FILT_BUF[ITEMS][(FILTER_NUM + 1)];
/* 滤波滑动窗口数组下标 */
uint8_t filter_cnt = 0,filter_cnt_old = 0;
/* 得出校准后的临时数据 */
float mpu6050_tmp[ITEMS];


/*----------------------------------------------------------
 + 实现功能：MPU6050加速度计、陀螺仪数据校准
----------------------------------------------------------*/
void MPU6050_Data_Offset()
{
    /* 加速度计校准 */
    if(mpu6050.Acc_CALIBRATE == 1)
    {
        /* 计数及累加 */
        acc_sum_cnt++;
        sum_temp[A_X] += mpu6050.Acc_I16.x;
        sum_temp[A_Y] += mpu6050.Acc_I16.y;
        sum_temp[A_Z] += mpu6050.Acc_I16.z - 4096;
        sum_temp[TEM] += mpu6050.Tempreature;

        /* 判断计数符合条件 */
        if( acc_sum_cnt >= OFFSET_AV_NUM )
        {
            /* 计算校验数据 */
            mpu6050.Acc_Offset.x = sum_temp[A_X]/OFFSET_AV_NUM;
            mpu6050.Acc_Offset.y = sum_temp[A_Y]/OFFSET_AV_NUM;
            mpu6050.Acc_Offset.z = sum_temp[A_Z]/OFFSET_AV_NUM;
            mpu6050.Acc_Temprea_Offset = sum_temp[TEM]/OFFSET_AV_NUM;
            /* 清零过程变量 */
            acc_sum_cnt =0;
            mpu6050.Acc_CALIBRATE = 0;
            sum_temp[A_X] = sum_temp[A_Y] = sum_temp[A_Z] = sum_temp[TEM] = 0;
        }
    }

    /* 陀螺仪校准 */
    if(mpu6050.Gyro_CALIBRATE)
    {
        /* 计数及累加 */
        gyro_sum_cnt++;
        sum_temp[G_X] += mpu6050.Gyro_I16.x;
        sum_temp[G_Y] += mpu6050.Gyro_I16.y;
        sum_temp[G_Z] += mpu6050.Gyro_I16.z;
        sum_temp[TEM] += mpu6050.Tempreature;

        /* 判断计数符合条件 */
        if( gyro_sum_cnt >= OFFSET_AV_NUM )
        {
            /* 计算校验数据 */
            mpu6050.Gyro_Offset.x = (float)sum_temp[G_X]/OFFSET_AV_NUM;
            mpu6050.Gyro_Offset.y = (float)sum_temp[G_Y]/OFFSET_AV_NUM;
            mpu6050.Gyro_Offset.z = (float)sum_temp[G_Z]/OFFSET_AV_NUM;
            mpu6050.Gyro_Temprea_Offset = sum_temp[TEM]/OFFSET_AV_NUM;
            /* 清零过程变量 */
            gyro_sum_cnt =0;
            mpu6050.Gyro_CALIBRATE = 0;
            sum_temp[G_X] = sum_temp[G_Y] = sum_temp[G_Z] = sum_temp[TEM] = 0;
        }
    }
}

/*----------------------------------------------------------
 + 实现功能：由任务调度调用周期2ms
----------------------------------------------------------*/
void Call_MPU6050_Data_Prepare(float T)
{
	  u8  i;
    /* 滤波滑动窗口临时数组 */
    s32 FILT_TMP[ITEMS] = {0,0,0,0,0,0,0};
    /* 原始数据读取临时数组 */
    u8 mpu6050_buffer[14];
    /* 陀螺仪原始数据临时变量 */
    float Gyro_tmp[3];

    /* 判断是否有MPU6050加速度计、陀螺仪数据校准 */
    MPU6050_Data_Offset();

		MPU_Get_Gyroscope(&mpu6050.Gyro_I16.x,&mpu6050.Gyro_I16.y,&mpu6050.Gyro_I16.z);
		MPU_Get_Accelerometer(&mpu6050.Acc_I16.x,&mpu6050.Acc_I16.y,&mpu6050.Acc_I16.z);

    /* 陀螺仪原始数据临时变量 */
    Gyro_tmp[0] = mpu6050.Gyro_I16.x ;
    Gyro_tmp[1] = mpu6050.Gyro_I16.y ;
    Gyro_tmp[2] = mpu6050.Gyro_I16.z ;

    /* 读取温度信息并滤波 */
    mpu6050.Tempreature = ((((int16_t)mpu6050_buffer[6]) << 8) | mpu6050_buffer[7]);
    mpu6050.TEM_LPF += 2 *3.14f *T *(mpu6050.Tempreature - mpu6050.TEM_LPF);
    mpu6050.Ftempreature = mpu6050.TEM_LPF/340.0f + 36.5f;

    /* 得出校准后的临时数据 */
    mpu6050_tmp[A_X] = (mpu6050.Acc_I16.x - mpu6050.Acc_Offset.x) ;
    mpu6050_tmp[A_Y] = (mpu6050.Acc_I16.y - mpu6050.Acc_Offset.y) ;
    mpu6050_tmp[A_Z] = (mpu6050.Acc_I16.z - mpu6050.Acc_Offset.z) ;
    mpu6050_tmp[G_X] = Gyro_tmp[0] - mpu6050.Gyro_Offset.x ;
    mpu6050_tmp[G_Y] = Gyro_tmp[1] - mpu6050.Gyro_Offset.y ;
    mpu6050_tmp[G_Z] = Gyro_tmp[2] - mpu6050.Gyro_Offset.z ;

    /* 滑动窗口滤波数组下标移位 */
    if( ++filter_cnt > FILTER_NUM )
    {
        filter_cnt = 0;
        filter_cnt_old = 1;
    }
    else
        filter_cnt_old = (filter_cnt == FILTER_NUM)? 0 : (filter_cnt + 1);

    /* 更新滤波滑动窗口临时数组 */
    FILT_BUF[A_X][filter_cnt] = mpu6050_tmp[A_X];
    FILT_BUF[A_Y][filter_cnt] = mpu6050_tmp[A_Y];
    FILT_BUF[A_Z][filter_cnt] = mpu6050_tmp[A_Z];
    FILT_BUF[G_X][filter_cnt] = mpu6050_tmp[G_X];
    FILT_BUF[G_Y][filter_cnt] = mpu6050_tmp[G_Y];
    FILT_BUF[G_Z][filter_cnt] = mpu6050_tmp[G_Z];
    /* 更新滤波滑动窗口数组 */
    for(i=0; i<FILTER_NUM; i++)
    {
        FILT_TMP[A_X] += FILT_BUF[A_X][i];
        FILT_TMP[A_Y] += FILT_BUF[A_Y][i];
        FILT_TMP[A_Z] += FILT_BUF[A_Z][i];
        FILT_TMP[G_X] += FILT_BUF[G_X][i];
        FILT_TMP[G_Y] += FILT_BUF[G_Y][i];
        FILT_TMP[G_Z] += FILT_BUF[G_Z][i];
    }

    /* 得出处理后的数据 */
    mpu6050.Acc.x  = (float)( FILT_TMP[A_X] )/(float)FILTER_NUM;
    mpu6050.Acc.y  = (float)( FILT_TMP[A_Y] )/(float)FILTER_NUM;
    mpu6050.Acc.z  = (float)( FILT_TMP[A_Z] )/(float)FILTER_NUM;
    mpu6050.Gyro.x  = (float)( FILT_TMP[G_X] )/(float)FILTER_NUM;
    mpu6050.Gyro.y  = (float)( FILT_TMP[G_Y] )/(float)FILTER_NUM;
    mpu6050.Gyro.z  = (float)( FILT_TMP[G_Z] )/(float)FILTER_NUM;

    /* 从数据换算到角速度 */
    mpu6050.Gyro_deg.x = mpu6050.Gyro.x *TO_ANGLE;
    mpu6050.Gyro_deg.y = mpu6050.Gyro.y *TO_ANGLE;
    mpu6050.Gyro_deg.z = mpu6050.Gyro.z *TO_ANGLE;
}

/*----------------------------------------------------------
 + 实现功能：判断是否磁力计校准
----------------------------------------------------------*/
void AK8975_Set_CalOffset(void)
{
    /* 校准前的缺省值 */
    static xyz_f_t	MagMAX = { -100 , -100 , -100 }, MagMIN = { 100 , 100 , 100 };
    /* 校准时间计数 */
    static uint16_t cnt_m=0;

    /* 判断是否磁力计校准 */
    if(Mag_CALIBRATED)
    {
        /* 校准时间计数 */
        cnt_m++;
        /* 保存单一方向最值 */
        if(ABS(ak8975.Mag_Adc.x)<400)
        {
            MagMAX.x = MAX(ak8975.Mag_Adc.x, MagMAX.x);
            MagMIN.x = MIN(ak8975.Mag_Adc.x, MagMIN.x);
        }
        /* 保存单一方向最值 */
        if(ABS(ak8975.Mag_Adc.y)<400)
        {
            MagMAX.y = MAX(ak8975.Mag_Adc.y, MagMAX.y);
            MagMIN.y = MIN(ak8975.Mag_Adc.y, MagMIN.y);
        }
        /* 保存单一方向最值 */
        if(ABS(ak8975.Mag_Adc.z)<400)
        {
            MagMAX.z = MAX(ak8975.Mag_Adc.z, MagMAX.z);
            MagMIN.z = MIN(ak8975.Mag_Adc.z, MagMIN.z);
        }
        /* 校准时间到 */
        if(cnt_m == CALIBRATING_MAG_CYCLES)
        {
            /* 保存校准后的数据 */
            ak8975.Mag_Offset.x = (MagMAX.x + MagMIN.x)/2;
            ak8975.Mag_Offset.y = (MagMAX.y + MagMIN.y)/2;
            ak8975.Mag_Offset.z = (MagMAX.z + MagMIN.z)/2;
            /* 校准、校准时间清零 */
            cnt_m = 0;
            Mag_CALIBRATED = false;
        }
    }
}

/*----------------------------------------------------------
 + 实现功能：由任务调度调用周期10ms
----------------------------------------------------------*/
void Call_AK8975(void)
{
    /* 滤波滑动窗口临时数组 */
    static xyz_f_t Mag_Adc_fill[MAG_FILTER_NUM];
    /* 滤波滑动窗口总和 */
    static xyz_f_t Mag_Adc_fill_total;
    /* 滤波滑动窗口数组下标 */
    static char temp_i;
    short temp[3];
	  MPU_Get_Magnetometer(&temp[0],&temp[1],&temp[2]);
    /* 磁力计采样原始数据 */
    ak8975.Mag_Adc.x =   temp[0];
    ak8975.Mag_Adc.y =   temp[1];
    ak8975.Mag_Adc.z =   temp[2];

    /* 更新滤波滑动窗口临时数组 */
    Mag_Adc_fill[temp_i].x = ak8975.Mag_Adc.x;
    Mag_Adc_fill[temp_i].y = ak8975.Mag_Adc.y;
    Mag_Adc_fill[temp_i].z = ak8975.Mag_Adc.z;
    /* 滑动窗口滤波数组下标移位 */
    temp_i++;
    if(temp_i>=MAG_FILTER_NUM)temp_i=0;

    /* 更新滑动窗口滤波数据总和 */
    Mag_Adc_fill_total.x+=ak8975.Mag_Adc.x;
    Mag_Adc_fill_total.y+=ak8975.Mag_Adc.y;
    Mag_Adc_fill_total.z+=ak8975.Mag_Adc.z;
    Mag_Adc_fill_total.x-=Mag_Adc_fill[temp_i].x;
    Mag_Adc_fill_total.y-=Mag_Adc_fill[temp_i].y;
    Mag_Adc_fill_total.z-=Mag_Adc_fill[temp_i].z;

    /* 得出处理后的数据 */
    ak8975.Mag_Val.x = Mag_Adc_fill_total.x / MAG_FILTER_NUM;
    ak8975.Mag_Val.y = Mag_Adc_fill_total.y / MAG_FILTER_NUM;
    ak8975.Mag_Val.z = Mag_Adc_fill_total.z / MAG_FILTER_NUM;

    /* 判断是否磁力计校准 */
    AK8975_Set_CalOffset();
}


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F746开发板
//MPU9250驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化MPU9250
//返回值:0,成功
//    其他,错误代码
u8 MPU9250_Init(void)
{
    u8 res=0;
    IIC_Init();     //初始化IIC总线
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU9250
    delay_ms(100);  //延时100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU9250
    MPU_Set_Gyro_Fsr(3);					        	//陀螺仪传感器,±2000dps
	  MPU_Set_Accel_Fsr(2);					       	 	//加速度传感器,±8g
    MPU_Set_Rate(1000);						       	 	//设置采样率1000Hz
    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //关闭所有中断
	  MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
	  MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //读取MPU6500的ID
    if(res==MPU6500_ID) //器件ID正确
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
		    MPU_Set_Rate(1000);						       	//设置采样率为1000Hz   
    }else{
			printf("err ID:%d\r\n",res);
		  return 1;
		}
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//读取AK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//设置AK8963为单次测量模式
    }else{
			printf("err mag ID:%d\r\n",res);
		  return 2;
		}
    return 0;
}

//设置MPU9250陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}
//设置MPU9250加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

//设置MPU9250的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器  
}

//设置MPU9250的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(42);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
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
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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


//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
	MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
	return res;;
}

//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    for(i=0;i<len;i++)
    {
        IIC_Send_Byte(buf[i]);  //发送数据
        if(IIC_Wait_Ack())      //等待ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
} 

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
	IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++;  
    }
    IIC_Stop();                 //产生一个停止条件
    return 0;       
}

//IIC写一个字节 
//devaddr:器件IIC地址
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    IIC_Send_Byte(data);        //发送数据
    if(IIC_Wait_Ack())          //等待ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    IIC_Wait_Ack();             //等待应答
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
	  IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    res=IIC_Read_Byte(0);		//读数据,发送nACK  
    IIC_Stop();                 //产生一个停止条件
    return res;  
}
