#include "ctrl.h"
#include "mpu9250.h"
#include "imu.h"
#include "ultrasound.h"
#include "usart.h"

ctrl_structure ctrl;

#define  MAX_ANGLE_ERR  50.0f
#define  MAX_SPEED      230.0f
#define  FIND_PWM       3000.0
#define  GYRO_TO_ANGLE    0.06103515625
#define  myabs(x) (x>0)?x:(-x)



//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{
	while((USART1->SR&0X40)==0);//等待上一次发送完毕   
	USART1->DR=c;   	
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


void ctrl_init(void)
{
  ctrl.mode = fiding_mode;
}

void system_ctrl(void)
{
	float target_speed,target_angle;
	float m1,m2,m3;
	       
//	uint8_t i;
  speed_measure();
	
	CH[ROLL]=rx_ch[0];
	CH[PITCH]=rx_ch[1];
	if(((CH[PITCH]>=1480)&&(CH[PITCH]<=1520))||(CH[PITCH]>2080)||(CH[PITCH]<980)) CH[PITCH]=1500;
  if(((CH[ROLL]>=1480)&&(CH[ROLL]<=1520))||(CH[ROLL]>2080)||(CH[ROLL]<980)) CH[ROLL]=1500;
	if(((CH[VRA]>=1480)&&(CH[VRA]<=1520))||(CH[VRA]>2080)||(CH[VRA]<980)) CH[VRA]=1500;
	
	if((CH[VRA]>1400)&&(CH[VRA]<=2200)) ctrl.mode = user_mode; 
	else  ctrl.mode = fiding_mode;
    
	enable_motor(true,true,true);
	
	target_speed = MAX_SPEED*((float)CH[PITCH]-1500.0f)/500.0f;
	target_angle = MAX_ANGLE_ERR*((float)CH[ROLL]-1500.0f)/500.0f;
  
//	printf("%6d %6d \r\n",CH[ROLL],CH[PITCH]);
	ctrl.mode = user_mode;
	if(ctrl.mode==user_mode){
//		 first_find=1;
	   if((target_angle>-20)&&(target_angle<20)){
			   target_angle = straight_angle - IMU_Yaw;
			   set_toward(target_angle,target_speed,-target_speed);
//			   if(CH[PITCH]==1500) straight_angle = IMU_Yaw;
			   m1 = turn_value(target_angle,mpu6050.Gyro.z);
			   m2 = speed_value(target_speed,0)+turn_value(target_angle,mpu6050.Gyro.z);		
  			 m3 = speed_value(target_speed,0)-turn_value(target_angle,mpu6050.Gyro.z);
			   straight_angle = IMU_Yaw;
			   if(CH[PITCH]==1500) m1 = 0;
		 }else{
		   set_toward(target_angle,target_angle,target_angle);
			 m1 = m2 = m3 = turn_value(target_angle,mpu6050.Gyro.z);
		 }
	}else if(ctrl.mode==fiding_mode){
    
	}
	
//	printf("%6d %6d %6d %6d %6d %6d\r\n",CH[ROLL],CH[PITCH],CH[THR],CH[YAW],CH[VRA],CH[VRB]);
//	printf("dis:%10lf\r\n",ultra_temp);
//	printf("dis:%10lf\r\n",ultra_distance);
//	usart1_report_imu(mpu6050.Acc.x,mpu6050.Acc.y,mpu6050.Acc.z,mpu6050.Gyro_deg.x,mpu6050.Gyro_deg.y,mpu6050.Gyro_deg.z
//	                 ,IMU_Roll*100,IMU_Pitch*100,IMU_Yaw*100);
//	printf("/*****************************************************/\r\n");
//	printf("remote :%d\r\n",remote_times);
//	printf("accel:%10lf %10lf %10lf\r\n",mpu6050.Acc.x,mpu6050.Acc.y,mpu6050.Acc.z);
//	printf("gyro:%10lf %10lf %10lf\r\n",mpu6050.Gyro_deg.x,mpu6050.Gyro_deg.y,mpu6050.Gyro_deg.z);
//	printf("imu :%10lf %10lf %10lf\r\n",IMU_Roll,IMU_Pitch,IMU_Yaw);
//	printf("mag:%10d %10d %10d \r\n",ak8975.Mag_Adc.x,ak8975.Mag_Adc.y,ak8975.Mag_Adc.z);
//	printf("/*****************************************************/\r\n\r\n\r\n\r\n\r\n");
	motor1.pwm = (uint16_t)myabs(m1);
	motor2.pwm = (uint16_t)myabs(m2);
	motor3.pwm = (uint16_t)myabs(m3);
	set_pwm(motor1.pwm,motor2.pwm,motor3.pwm);
}

float turn_value(float err_angle,float gyroz)
{
  float kp,kd;
	float turn_pwm;
	kp = 120;  kd = 1.5;
	if(err_angle>MAX_ANGLE_ERR) err_angle = MAX_ANGLE_ERR;
	else if(err_angle<-MAX_ANGLE_ERR) err_angle = -MAX_ANGLE_ERR;//对目标角度限幅
	
	gyroz = gyroz*GYRO_TO_ANGLE; // -2000 ---  +2000
	turn_pwm = err_angle*kp + gyroz*kd;
	return turn_pwm;
}

float speed_value(float target,float sample)
{
  float speed_pwm=0;
	float amplitute=5000;
	if((CH[PITCH]>=1480)&&(CH[PITCH]<=1520)) speed_pwm=0;
	speed_pwm = amplitute*(target - sample)/MAX_SPEED;
	return speed_pwm;
}


void enable_motor(bool st1,bool st2,bool st3)
{
  if(st1) motor1.enable = true;
	else motor1.enable = false;
	if(st2) motor2.enable = true;
	else motor2.enable = false;
	if(st3) motor3.enable = true;
	else motor3.enable = false;
}

void set_toward(float m1,float m2,float m3)
{
  if(m1>0) motor1.toward = postive;
	else motor1.toward = negtive;
	 if(m2>0) motor2.toward = postive;
	else motor2.toward = negtive;
	 if(m3>0) motor3.toward = postive;
	else motor3.toward = negtive;
}

