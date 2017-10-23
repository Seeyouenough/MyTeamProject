#include "driver_mpu6050.h"


/* calibrate times */
#define CALIBRATE_NUM   50 

/*gyro adc adding 1,inform degree speed adding 16.4 degree in full range of +-2000
  note 1: for different full range,this value is different												
	note 2: here multiply 0.06103 rather than divide 16.4 ,because dealing more fast */
#define GYRO_TO_ANGLE    0.06103f


mpu6050_structure mpu6050;

//initialize MPU6050
//return:0,successed
//others,error code
uint8_t mpu_init(void)
{
	senser_iic_init();
	mpu6050_int_config();  //at first config the pin of iic and interrupt_pin
	
	if(mpu_power_manegment(0x80)) {//reset mpu6050
	  printf("power reset fail!\r\n");
	}
	delay_ms(100);
	if(mpu_power_manegment(0x00)) {//wake up mpu6050
	  printf("power wake up fail!\r\n");
	}
	delay_ms(10);
	if(mpu_set_sample_rate(1000)){ //1000 Hz sample rate
	  printf("sample rate setting fail!\r\n");
	}
	delay_ms(10);
	if(mpu_set_gyro_fsr(GYRO_FSR_2000)){  //+- 2000 degree
	  printf("gyroscope fullscale range setting fail!\r\n");
	}
	delay_ms(10);
	if(mpu_set_accel_fsr(ACCE_FSR_8_G)){  //+- 8g
	  printf("accelerate fullscale range setting fail!\r\n");  
	}
	delay_ms(10);
	if(mpu_set_lpf(LPF_BAND_WIDTH_20)){  //42 Hz LPF
	  printf("LPF setting failed!\r\n");
	}
	delay_ms(10);
	if(mpu_set_iic_master(0x00)){        //disable 6050 control AUX_IIC
	  printf("master mode setting failed\r\n");
	}      
	delay_ms(10);
	if(mpu_int_bypass_config(0x02)){    //enable bypass mode
	  printf("bypass config failed!\r\n");
	}
	delay_ms(10);
	if(senser_read_byte(MPU_ADDR,MPU_DEVICE_ID_REG)==0x68)  //6050 device ID was 0x68
	return 0;
	else {
		printf("DEVICE ID:%d\r\n",senser_read_byte(MPU_ADDR,MPU_DEVICE_ID_REG));
	  return 1;
	}
}

void mpu6050_calibrate(void)
{
	static int32_t accel_temp[3]={0,0,0},gyro_temp[3]={0,0,0},accel_cnt=0,gyro_cnt=0;
	/*accelemeter calibrating*/
  if(mpu6050.accel_calibrate==true){
	  accel_cnt++;
		accel_temp[0] += mpu6050.accel_adc.x;
		accel_temp[1] += mpu6050.accel_adc.y;
		accel_temp[2] += mpu6050.accel_adc.z - 4096;
		
		/*reach num of calibrate*/
		if(accel_cnt>=CALIBRATE_NUM){
		  mpu6050.accel_offset.x = (float)accel_temp[0]/(float)accel_cnt;
			mpu6050.accel_offset.y = (float)accel_temp[1]/(float)accel_cnt;
			mpu6050.accel_offset.z = (float)accel_temp[2]/(float)accel_cnt;
			
			accel_cnt = 0;
			accel_temp[0] =0; accel_temp[1] = 0; accel_temp[2] = 0;
			mpu6050.accel_calibrate = false;
		}
	}
	/*gyroscope calibrating*/
	if(mpu6050.gyro_calibrate == true){
	  gyro_cnt++;
		gyro_temp[0] += mpu6050.gyro_adc.x;
		gyro_temp[1] += mpu6050.gyro_adc.y;
		gyro_temp[2] += mpu6050.gyro_adc.z;
		
		if(gyro_cnt>=CALIBRATE_NUM){
		  mpu6050.gyro_offset.x = (float)gyro_temp[0]/(float)gyro_cnt;
			mpu6050.gyro_offset.y = (float)gyro_temp[1]/(float)gyro_cnt;
			mpu6050.gyro_offset.z = (float)gyro_temp[2]/(float)gyro_cnt;
			gyro_cnt = 0;
			mpu6050.gyro_calibrate = false;
			gyro_temp[0] = 0; gyro_temp[1] = 0; gyro_temp[2] = 0;
		}
	}
}

/*
  founctions: prepare the accel-gyro sensor data for imu status calculate
  input params:interval time between two calling
*/
void mpu6050_data_prepare(void)
{
  uint8_t sensor_temp[14];
	static  int32_t sensor_sum[6]={0,0,0,0,0,0};
	uint8_t i;
	static  int16_t filter_temp[6][10]={0};//saving last 10 times data,get average value
	
	mpu6050_calibrate();
	senser_read_len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,14,sensor_temp); /*reading data from mpu6050*/
	mpu6050.accel_adc.x = (sensor_temp[0]<<8)|sensor_temp[1];
	mpu6050.accel_adc.y = (sensor_temp[2]<<8)|sensor_temp[3];
	mpu6050.accel_adc.z = (sensor_temp[4]<<8)|sensor_temp[5];
	
	mpu6050.gyro_adc.x =  (sensor_temp[8]<<8)|sensor_temp[9];
	mpu6050.gyro_adc.y =  (sensor_temp[10]<<8)|sensor_temp[11];
	mpu6050.gyro_adc.z =  (sensor_temp[12]<<8)|sensor_temp[13];
	
	mpu6050.tempreture_adc = (sensor_temp[6]<<8)|sensor_temp[7];
	mpu6050.tempreture_lpf += 2*3.14f*0.002*(mpu6050.tempreture_adc - mpu6050.tempreture_lpf);
	mpu6050.tempreture = mpu6050.tempreture_lpf/340.0f + 36.5f;
	
	for(i=0;i<6;i++){
	   sensor_sum[i] -= filter_temp[i][0];
	}
	
	for(i=0;i<9;i++){
	   filter_temp[0][i] = filter_temp[0][i+1];
		 filter_temp[1][i] = filter_temp[1][i+1];
		 filter_temp[2][i] = filter_temp[2][i+1];
		 filter_temp[3][i] = filter_temp[3][i+1];
		 filter_temp[4][i] = filter_temp[4][i+1];
		 filter_temp[5][i] = filter_temp[5][i+1];
	}
	filter_temp[0][9] = (float)mpu6050.accel_adc.x - mpu6050.accel_offset.x;
	filter_temp[1][9] = (float)mpu6050.accel_adc.y - mpu6050.accel_offset.y;
	filter_temp[2][9] = (float)mpu6050.accel_adc.z - mpu6050.accel_offset.z;
	filter_temp[3][9] = (float)mpu6050.gyro_adc.x - mpu6050.gyro_offset.x;
	filter_temp[4][9] = (float)mpu6050.gyro_adc.y - mpu6050.gyro_offset.y;
	filter_temp[5][9] = (float)mpu6050.gyro_adc.z - mpu6050.gyro_offset.z;
  for(i=0;i<6;i++){
	   sensor_sum[i] += filter_temp[i][9];
	}
	
	mpu6050.accel.x = 0.03f*sensor_sum[0]+0.7f*filter_temp[0][9];
	mpu6050.accel.y = 0.03f*sensor_sum[1]+0.7f*filter_temp[1][9];
	mpu6050.accel.z = 0.03f*sensor_sum[2]+0.7f*filter_temp[2][9];
	mpu6050.gyro.x = 0.03f*sensor_sum[3]+0.7f*filter_temp[3][9];
	mpu6050.gyro.y = 0.03f*sensor_sum[4]+0.7f*filter_temp[4][9];
	mpu6050.gyro.z = 0.03f*sensor_sum[5]+0.7f*filter_temp[5][9];
	
	mpu6050.gyro_degree.x = mpu6050.gyro.x*GYRO_TO_ANGLE;
	mpu6050.gyro_degree.y = mpu6050.gyro.y*GYRO_TO_ANGLE;
	mpu6050.gyro_degree.z = mpu6050.gyro.z*GYRO_TO_ANGLE;
////	printf("%12lf %12lf %12lf\r\n",mpu6050.gyro_degree.x,mpu6050.gyro_degree.y,mpu6050.gyro_degree.z);
//	usart1_report_imu((int16_t)mpu6050.accel_offset.x,(int16_t)mpu6050.accel_offset.y,(int16_t)mpu6050.accel_offset.z,
//	                  (int16_t)mpu6050.gyro.x,(int16_t)mpu6050.gyro.y,(int16_t)mpu6050.gyro.z,109,203,304);
}


//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{
	while((USART3->SR&0X40)==0);//等待上一次发送完毕   
	USART3->DR=c;   	
} 
//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//发送数据到串口1 
}
//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	usart1_niming_report(0XA1,tbuf,12);//自定义帧,0XA1
}	
//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
}   

