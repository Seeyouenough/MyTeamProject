#include "mpu9250.h" 
#include "math.h"

#define Kp 0.6f // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.1f // 0.001  integral gain governs rate of convergence of gyroscope biases
/* 数学函数常量 */
#define ONE_PI   (3.14159265)
#define TWO_PI   (2.0 * 3.14159265)
#define ANGLE_UNIT (TWO_PI/10.0)
/* (smallest non-zero value in table) */
#define TAN_MAP_RES     0.003921569f
#define RAD_PER_DEG     0.017453293f
#define TAN_MAP_SIZE    256

/* 角度转弧度比例 */
#define ANGLE_TO_RADIAN 0.01745329f
/* 2倍角度转弧度比例 */
#define IMU_INTEGRAL_LIM  ( 2.0f *ANGLE_TO_RADIAN )
/* 加速计均方根积分滤波常量 */
#define NORM_ACC_LPF_HZ 10
/* 误差积分滤波常量 */
#define REF_ERR_LPF_HZ  1

/* 加速度：由下向上方向的加速度在加速度计的分量 */
xyz_f_t reference_v;
/* 加速度：由南向北方向的加速度在加速度计的分量 *//* 加速度：由东向西方向的加速度在加速度计的分量 */
xyz_f_t north,west;
/* 数据处理过程量结构体 */
ref_t 	ref;

/* 最终计算出的姿态 单位 角度 */
float IMU_Roll,IMU_Pitch,IMU_Yaw;

/* 四元数的W X Y Z */
float ref_q[4] = {1,0,0,0};

/* 加速计均方根 *//* 四元数均方根 */
float norm_acc,norm_q;
/* 加速计均方根积分滤波 */
float norm_acc_lpf;

/* 磁力计均方根 */ /* 磁力计数据 除以 均方根 */
float mag_norm ,mag_norm_xyz ;

/* 匹配好方位的磁力计数据 */
xyz_f_t mag_sim_3d;


/*----------------------------------------------------------
 + 实现功能：引用加速度计分量作为参考，磁力方位匹配计算
----------------------------------------------------------*/
void mag_3d_trans(xyz_f_t *ref, xyz_f_t *in, xyz_f_t *out)
{
	static s8 pn;
	static float h_tmp_x,h_tmp_y;

  h_tmp_x = sqrt((ref->z)*(ref->z) + (ref->y)*(ref->y));
	h_tmp_y = sqrt((ref->z)*(ref->z) + (ref->x)*(ref->x));

	pn = ref->z < 0? -1 : 1;

	out->x = ( h_tmp_x *in->x - pn *ref->x *in->z ) ;
	out->y = ( pn *h_tmp_y *in->y - ref->y *in->z ) ;
	out->z = ref->x *in->x + ref->y *in->y + ref->z *in->z ;
}
/*----------------------------------------------------------
 + 实现功能：角度范围控制在+-180角度
----------------------------------------------------------*/
float To_180_degrees(float x)
{
    return ( x>180 ? (x-360) : ( x<-180 ? (x+360) : x ) );
}
/*----------------------------------------------------------
 + 实现功能：绝对值计算
----------------------------------------------------------*/
float my_abs(float f)
{
	if (f >= 0.0f)
	{
		return f;
	}

	return -f;
}

/* 快速反正切计算常量数组 */
float fast_atan_table[257] =
{
	0.000000e+00, 3.921549e-03, 7.842976e-03, 1.176416e-02,
	1.568499e-02, 1.960533e-02, 2.352507e-02, 2.744409e-02,
	3.136226e-02, 3.527947e-02, 3.919560e-02, 4.311053e-02,
	4.702413e-02, 5.093629e-02, 5.484690e-02, 5.875582e-02,
	6.266295e-02, 6.656816e-02, 7.047134e-02, 7.437238e-02,
	7.827114e-02, 8.216752e-02, 8.606141e-02, 8.995267e-02,
	9.384121e-02, 9.772691e-02, 1.016096e-01, 1.054893e-01,
	1.093658e-01, 1.132390e-01, 1.171087e-01, 1.209750e-01,
	1.248376e-01, 1.286965e-01, 1.325515e-01, 1.364026e-01,
	1.402496e-01, 1.440924e-01, 1.479310e-01, 1.517652e-01,
	1.555948e-01, 1.594199e-01, 1.632403e-01, 1.670559e-01,
	1.708665e-01, 1.746722e-01, 1.784728e-01, 1.822681e-01,
	1.860582e-01, 1.898428e-01, 1.936220e-01, 1.973956e-01,
	2.011634e-01, 2.049255e-01, 2.086818e-01, 2.124320e-01,
	2.161762e-01, 2.199143e-01, 2.236461e-01, 2.273716e-01,
	2.310907e-01, 2.348033e-01, 2.385093e-01, 2.422086e-01,
	2.459012e-01, 2.495869e-01, 2.532658e-01, 2.569376e-01,
	2.606024e-01, 2.642600e-01, 2.679104e-01, 2.715535e-01,
	2.751892e-01, 2.788175e-01, 2.824383e-01, 2.860514e-01,
	2.896569e-01, 2.932547e-01, 2.968447e-01, 3.004268e-01,
	3.040009e-01, 3.075671e-01, 3.111252e-01, 3.146752e-01,
	3.182170e-01, 3.217506e-01, 3.252758e-01, 3.287927e-01,
	3.323012e-01, 3.358012e-01, 3.392926e-01, 3.427755e-01,
	3.462497e-01, 3.497153e-01, 3.531721e-01, 3.566201e-01,
	3.600593e-01, 3.634896e-01, 3.669110e-01, 3.703234e-01,
	3.737268e-01, 3.771211e-01, 3.805064e-01, 3.838825e-01,
	3.872494e-01, 3.906070e-01, 3.939555e-01, 3.972946e-01,
	4.006244e-01, 4.039448e-01, 4.072558e-01, 4.105574e-01,
	4.138496e-01, 4.171322e-01, 4.204054e-01, 4.236689e-01,
	4.269229e-01, 4.301673e-01, 4.334021e-01, 4.366272e-01,
	4.398426e-01, 4.430483e-01, 4.462443e-01, 4.494306e-01,
	4.526070e-01, 4.557738e-01, 4.589307e-01, 4.620778e-01,
	4.652150e-01, 4.683424e-01, 4.714600e-01, 4.745676e-01,
	4.776654e-01, 4.807532e-01, 4.838312e-01, 4.868992e-01,
	4.899573e-01, 4.930055e-01, 4.960437e-01, 4.990719e-01,
	5.020902e-01, 5.050985e-01, 5.080968e-01, 5.110852e-01,
	5.140636e-01, 5.170320e-01, 5.199904e-01, 5.229388e-01,
	5.258772e-01, 5.288056e-01, 5.317241e-01, 5.346325e-01,
	5.375310e-01, 5.404195e-01, 5.432980e-01, 5.461666e-01,
	5.490251e-01, 5.518738e-01, 5.547124e-01, 5.575411e-01,
	5.603599e-01, 5.631687e-01, 5.659676e-01, 5.687566e-01,
	5.715357e-01, 5.743048e-01, 5.770641e-01, 5.798135e-01,
	5.825531e-01, 5.852828e-01, 5.880026e-01, 5.907126e-01,
	5.934128e-01, 5.961032e-01, 5.987839e-01, 6.014547e-01,
	6.041158e-01, 6.067672e-01, 6.094088e-01, 6.120407e-01,
	6.146630e-01, 6.172755e-01, 6.198784e-01, 6.224717e-01,
	6.250554e-01, 6.276294e-01, 6.301939e-01, 6.327488e-01,
	6.352942e-01, 6.378301e-01, 6.403565e-01, 6.428734e-01,
	6.453808e-01, 6.478788e-01, 6.503674e-01, 6.528466e-01,
	6.553165e-01, 6.577770e-01, 6.602282e-01, 6.626701e-01,
	6.651027e-01, 6.675261e-01, 6.699402e-01, 6.723452e-01,
	6.747409e-01, 6.771276e-01, 6.795051e-01, 6.818735e-01,
	6.842328e-01, 6.865831e-01, 6.889244e-01, 6.912567e-01,
	6.935800e-01, 6.958943e-01, 6.981998e-01, 7.004964e-01,
	7.027841e-01, 7.050630e-01, 7.073330e-01, 7.095943e-01,
	7.118469e-01, 7.140907e-01, 7.163258e-01, 7.185523e-01,
	7.207701e-01, 7.229794e-01, 7.251800e-01, 7.273721e-01,
	7.295557e-01, 7.317307e-01, 7.338974e-01, 7.360555e-01,
	7.382053e-01, 7.403467e-01, 7.424797e-01, 7.446045e-01,
	7.467209e-01, 7.488291e-01, 7.509291e-01, 7.530208e-01,
	7.551044e-01, 7.571798e-01, 7.592472e-01, 7.613064e-01,
	7.633576e-01, 7.654008e-01, 7.674360e-01, 7.694633e-01,
	7.714826e-01, 7.734940e-01, 7.754975e-01, 7.774932e-01,
	7.794811e-01, 7.814612e-01, 7.834335e-01, 7.853983e-01,
	7.853983e-01
};

/*----------------------------------------------------------
 + 实现功能：快速反正切计算
----------------------------------------------------------*/
float fast_atan2(float y, float x)
{
	float x_abs, y_abs, z;
	float alpha, angle, base_angle;
	int index;

	/* don't divide by zero! */
	if ((y == 0.0f) && (x == 0.0f))
		angle = 0.0f;
	else
	{
		/* normalize to +/- 45 degree range */
		y_abs = my_abs(y);
		x_abs = my_abs(x);
		if (y_abs < x_abs)
			z = y_abs / x_abs;
		else
			z = x_abs / y_abs;
		/* when ratio approaches the table resolution, the angle is */
		/*      best approximated with the argument itself...       */
		if (z < TAN_MAP_RES)
			base_angle = z;
		else
		{
			/* find index and interpolation value */
			alpha = z * (float) TAN_MAP_SIZE - .5f;
			index = (int) alpha;
			alpha -= (float) index;
			/* determine base angle based on quadrant and */
			/* add or subtract table value from base angle based on quadrant */
			base_angle = fast_atan_table[index];
			base_angle += (fast_atan_table[index + 1] - fast_atan_table[index]) * alpha;
		}

		if (x_abs > y_abs)
		{        /* -45 -> 45 or 135 -> 225 */
			if (x >= 0.0f)
			{           /* -45 -> 45 */
				if (y >= 0.0f)
					angle = base_angle;   /* 0 -> 45, angle OK */
				else
					angle = -base_angle;  /* -45 -> 0, angle = -angle */
			}
			else
			{                  /* 135 -> 180 or 180 -> -135 */
				angle = 3.14159265358979323846;

				if (y >= 0.0f)
					angle -= base_angle;  /* 135 -> 180, angle = 180 - angle */
				else
					angle = base_angle - angle;   /* 180 -> -135, angle = angle - 180 */
			}
		}
		else
		{                    /* 45 -> 135 or -135 -> -45 */
			if (y >= 0.0f)
			{           /* 45 -> 135 */
				angle = 1.57079632679489661923;

				if (x >= 0.0f)
					angle -= base_angle;  /* 45 -> 90, angle = 90 - angle */
				else
					angle += base_angle;  /* 90 -> 135, angle = 90 + angle */
			}
			else
			{                  /* -135 -> -45 */
				angle = -1.57079632679489661923;

				if (x >= 0.0f)
					angle += base_angle;  /* -90 -> -45, angle = -90 + angle */
				else
					angle -= base_angle;  /* -135 -> -90, angle = -90 - angle */
			}
		}
	}
	return (angle);
}

/*----------------------------------------------------------
 + 实现功能：由任务调度调用周期5ms
 + 调用参数：两次调用时间差的一半
----------------------------------------------------------*/
void Call_IMUupdate(float half_T)
{
	/* 陀螺仪数据赋值 */
	float gx = mpu6050.Gyro_deg.x;
	float gy = mpu6050.Gyro_deg.y;
	float gz = mpu6050.Gyro_deg.z;
	/* 加速度计数据赋值 */
	float ax = mpu6050.Acc.x;
	float ay = mpu6050.Acc.y;
	float az = mpu6050.Acc.z;
	/* 误差积分滤波比例 */
	float ref_err_lpf_hz;
	static float yaw_correct;
	static float yaw_mag;

	static xyz_f_t mag_tmp;
	float mag_norm_tmp;

	/* 积分滤波比例 */
	mag_norm_tmp = 20 *(6.28f *half_T);

	/* 磁力计均方根计算 */
  mag_norm_xyz = sqrt(ak8975.Mag_Val.x * ak8975.Mag_Val.x + ak8975.Mag_Val.y * ak8975.Mag_Val.y + ak8975.Mag_Val.z * ak8975.Mag_Val.z);
	/* 数据正常 */
	if( mag_norm_xyz != 0)
	{
			/* 磁力计分量 均方根计算 */
			mag_tmp.x += mag_norm_tmp *( (float)ak8975.Mag_Val.x /( mag_norm_xyz ) - mag_tmp.x);
			mag_tmp.y += mag_norm_tmp *( (float)ak8975.Mag_Val.y /( mag_norm_xyz ) - mag_tmp.y);
			mag_tmp.z += mag_norm_tmp *( (float)ak8975.Mag_Val.z /( mag_norm_xyz ) - mag_tmp.z);
	}

	/* 引用加速度计分量作为参考，磁力方位匹配计算 */
	mag_3d_trans(&reference_v,&mag_tmp,&mag_sim_3d);

	/* 匹配好方位的磁力计数据 均方根计算 */
	mag_norm = sqrt(mag_sim_3d.x * mag_sim_3d.x + mag_sim_3d.y *mag_sim_3d.y);
	/* 数据正常 */
	if( mag_sim_3d.x != 0 && mag_sim_3d.y != 0 && mag_sim_3d.z != 0 && mag_norm != 0)
	/* 计算Y航向角度 单位角度 */
	yaw_mag = fast_atan2( ( mag_sim_3d.x/mag_norm ) , ( mag_sim_3d.y/mag_norm) ) *57.3f;

	/* 加速度：由下向上方向的加速度在加速度计X分量 */
	reference_v.x = 2*(ref_q[1]*ref_q[3] - ref_q[0]*ref_q[2]);
	/* 加速度：由下向上方向的加速度在加速度计Y分量 */
	reference_v.y = 2*(ref_q[2]*ref_q[3] + ref_q[0]*ref_q[1]);
	/* 加速度：由下向上方向的加速度在加速度计Z分量 */
	reference_v.z = 1 - 2*(ref_q[1]*ref_q[1] + ref_q[2]*ref_q[2]);
	/* 加速度：由南向北方向的加速度在加速度计X分量 */
	north.x = 1 - 2*(ref_q[3]*ref_q[3] + ref_q[2]*ref_q[2]);
	/* 加速度：由南向北方向的加速度在加速度计Y分量 */
	north.y = 2* (-ref_q[0]*ref_q[3] + ref_q[1]*ref_q[2]);
	/* 加速度：由南向北方向的加速度在加速度计Z分量 */
	north.z = 2* (+ref_q[0]*ref_q[2]  - ref_q[1]*ref_q[3]);
	/* 加速度：由东向西方向的加速度在加速度计X分量 */
	west.x = 2* (+ref_q[0]*ref_q[3] + ref_q[1]*ref_q[2]);
	/* 加速度：由东向西方向的加速度在加速度计Y分量 */
	west.y = 1 - 2*(ref_q[3]*ref_q[3] + ref_q[1]*ref_q[1]);
	/* 加速度：由东向西方向的加速度在加速度计Z分量 */
	west.z = 2* (-ref_q[0]*ref_q[1] + ref_q[2]*ref_q[3]);

	/* 加速计均方根 估值约4096 */
	norm_acc = sqrt(ax*ax + ay*ay + az*az);
	/* 加速计均方根积分滤波 估值约4096 */
	norm_acc_lpf +=  NORM_ACC_LPF_HZ *(6.28f *half_T) *(norm_acc - norm_acc_lpf);

	/* 判断加速度计数据范围 */
	if(ABS(ax)<4400 && ABS(ay)<4400 && ABS(az)<4400 )
	{

			/* 加速度计数值 除以加速计均方根 */
			ax = ax / norm_acc;
			ay = ay / norm_acc;
			az = az / norm_acc;
			/* 数据正常 */
			if( 3800 < norm_acc && norm_acc < 4400 )
			{
			/* 叉乘得到误差 */
			ref.err_tmp.x = ay*reference_v.z - az*reference_v.y;
			ref.err_tmp.y = az*reference_v.x - ax*reference_v.z;
			/* 误差积分滤波比例 */
			ref_err_lpf_hz = REF_ERR_LPF_HZ *(6.28f *half_T);
			/* 误差积分滤波 */
			ref.err_lpf.x += ref_err_lpf_hz *( ref.err_tmp.x  - ref.err_lpf.x );
			ref.err_lpf.y += ref_err_lpf_hz *( ref.err_tmp.y  - ref.err_lpf.y );
			ref.err.x = ref.err_lpf.x;
			ref.err.y = ref.err_lpf.y;
			}
	}
	/* 数据异常 */
	else
	{
			ref.err.x = 0 ;
			ref.err.y = 0 ;
	}
	/* 误差积分 */
	ref.err_Int.x += ref.err.x *Ki *2 *half_T ;
	ref.err_Int.y += ref.err.y *Ki *2 *half_T ;
	ref.err_Int.z += ref.err.z *Ki *2 *half_T ;
	 /* 积分限幅 */
	ref.err_Int.x = LIMIT(ref.err_Int.x, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	ref.err_Int.y = LIMIT(ref.err_Int.y, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	ref.err_Int.z = LIMIT(ref.err_Int.z, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	/* 方向朝上 */
	if( reference_v.z >= 0.0f )
	{
			/* 已经解锁，防止磁力计受干扰只可低速融合 */
			if( false )
			{
					/* 磁力计计算姿态按较小权重算融合 */
					yaw_correct = Kp *0.2f *To_180_degrees(yaw_mag - IMU_Yaw);
			}
			/* 没有解锁，视作开机时刻，快速融合 */
			else
			{
					/* 磁力计计算姿态按较大权重算融合 */
					yaw_correct = To_180_degrees(yaw_mag - IMU_Yaw);
			}
	}
	/* 方向朝下 */
	else
	{
			yaw_mag+=180.0f;
			To_180_degrees(yaw_mag);

			/* 已经解锁，防止磁力计受干扰只可低速融合 */
			if( false )
			{
					/* 磁力计计算姿态按较小权重算融合 */
					yaw_correct = Kp *0.2f *To_180_degrees(yaw_mag - IMU_Yaw);
			}
			/* 没有解锁，视作开机时刻，快速融合 */
			else
			{
					/* 磁力计计算姿态按较大权重算融合 */
					yaw_correct = To_180_degrees(yaw_mag - IMU_Yaw);
			}
	}

	/* 引用陀螺仪数据  */
	ref.g.x = (gx - reference_v.x *yaw_correct) *ANGLE_TO_RADIAN + ( Kp*(ref.err.x + ref.err_Int.x) ) ;
	ref.g.y = (gy - reference_v.y *yaw_correct) *ANGLE_TO_RADIAN + ( Kp*(ref.err.y + ref.err_Int.y) ) ;
	ref.g.z = (gz - reference_v.z *yaw_correct) *ANGLE_TO_RADIAN;

	/* 更新四元数 */
	ref_q[0] = ref_q[0] +(-ref_q[1]*ref.g.x - ref_q[2]*ref.g.y - ref_q[3]*ref.g.z)*half_T;
	ref_q[1] = ref_q[1] + (ref_q[0]*ref.g.x + ref_q[2]*ref.g.z - ref_q[3]*ref.g.y)*half_T;
	ref_q[2] = ref_q[2] + (ref_q[0]*ref.g.y - ref_q[1]*ref.g.z + ref_q[3]*ref.g.x)*half_T;
	ref_q[3] = ref_q[3] + (ref_q[0]*ref.g.z + ref_q[1]*ref.g.y - ref_q[2]*ref.g.x)*half_T;

	/* 计算四元数均方根 */
  norm_q = sqrt(ref_q[0]*ref_q[0] + ref_q[1]*ref_q[1] + ref_q[2]*ref_q[2] + ref_q[3]*ref_q[3]);
	
	/* 四元数 除以四元数均方根，保证范围1以内 */
	ref_q[0] = ref_q[0] / norm_q;
	ref_q[1] = ref_q[1] / norm_q;
	ref_q[2] = ref_q[2] / norm_q;
	ref_q[3] = ref_q[3] / norm_q;

	/* 四元数转换到欧拉角 算法 */
	IMU_Roll = fast_atan2(2*(ref_q[0]*ref_q[1] + ref_q[2]*ref_q[3]),1 - 2*(ref_q[1]*ref_q[1] + ref_q[2]*ref_q[2])) *57.3f ;
	IMU_Pitch = asin(2*(ref_q[1]*ref_q[3] - ref_q[0]*ref_q[2])) *57.3f ;
	IMU_Yaw = fast_atan2(2*(-ref_q[1]*ref_q[2] - ref_q[0]*ref_q[3]), 2*(ref_q[0]*ref_q[0] + ref_q[1]*ref_q[1]) - 1) *57.3f ;
}

