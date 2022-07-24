/*
 * @Author: TOTHTOT
 * @Date: 2022-04-04 20:15:40
 * @LastEditTime: 2022-07-24 14:15:23
 * @LastEditors: TOTHTOT
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4轮小车\HARDWARE\PID\pid.c
 */
#include "pid.h"
#include "tb6612.h"

// _pid Pid_Speed1, Car_1.motro_state[1].seppd, Car_1.motro_state[1].seppd, Car_1.motro_state[1].seppd;
// _pid Car_1.motro_state[0].location, Car_1.motro_state[1].location, Car_1.motro_state[2].location, Car_1.motro_state[3].location;

void PID_Init(void)
{
    // _pid *speed_pid, _pid *location_pid
    /* 位置相关初始化参数 */
    Car_1.motro_state[0].location.target_val = 0.0;
    Car_1.motro_state[0].location.actual_val = 0.0;
    Car_1.motro_state[0].location.err = 0.0;
    Car_1.motro_state[0].location.err_last = 0.0;
    Car_1.motro_state[0].location.integral = 0.0;

    Car_1.motro_state[0].location.Kp = 0.24;
    Car_1.motro_state[0].location.Ki = 0.0;
    Car_1.motro_state[0].location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro_state[0].seppd.target_val = 0.0;
    Car_1.motro_state[0].seppd.actual_val = 0.0;
    Car_1.motro_state[0].seppd.err = 0.0;
    Car_1.motro_state[0].seppd.err_last = 0.0;
    Car_1.motro_state[0].seppd.integral = 0.0;

    Car_1.motro_state[0].seppd.Kp = 1.0;
    Car_1.motro_state[0].seppd.Ki = 0.4;
    Car_1.motro_state[0].seppd.Kd = 1.5;

    /* 位置相关初始化参数 */
    Car_1.motro_state[1].location.target_val = 0.0;
    Car_1.motro_state[1].location.actual_val = 0.0;
    Car_1.motro_state[1].location.err = 0.0;
    Car_1.motro_state[1].location.err_last = 0.0;
    Car_1.motro_state[1].location.integral = 0.0;

    Car_1.motro_state[1].location.Kp = 0.24;
    Car_1.motro_state[1].location.Ki = 0.0;
    Car_1.motro_state[1].location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro_state[1].seppd.target_val = 0.0;
    Car_1.motro_state[1].seppd.actual_val = 0.0;
    Car_1.motro_state[1].seppd.err = 0.0;
    Car_1.motro_state[1].seppd.err_last = 0.0;
    Car_1.motro_state[1].seppd.integral = 0.0;

    Car_1.motro_state[1].seppd.Kp = 1.0;
    Car_1.motro_state[1].seppd.Ki = 0.4;
    Car_1.motro_state[1].seppd.Kd = 1.5;
    
    /* 位置相关初始化参数 */
    Car_1.motro_state[2].location.target_val = 0.0;
    Car_1.motro_state[2].location.actual_val = 0.0;
    Car_1.motro_state[2].location.err = 0.0;
    Car_1.motro_state[2].location.err_last = 0.0;
    Car_1.motro_state[2].location.integral = 0.0;

    Car_1.motro_state[2].location.Kp = 0.24;
    Car_1.motro_state[2].location.Ki = 0.0;
    Car_1.motro_state[2].location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro_state[2].seppd.target_val = 0.0;
    Car_1.motro_state[2].seppd.actual_val = 0.0;
    Car_1.motro_state[2].seppd.err = 0.0;
    Car_1.motro_state[2].seppd.err_last = 0.0;
    Car_1.motro_state[2].seppd.integral = 0.0;

    Car_1.motro_state[2].seppd.Kp = 1.0;
    Car_1.motro_state[2].seppd.Ki = 0.4;
    Car_1.motro_state[2].seppd.Kd = 1.5;

    /* 位置相关初始化参数 */
    Car_1.motro_state[3].location.target_val = 0.0;
    Car_1.motro_state[3].location.actual_val = 0.0;
    Car_1.motro_state[3].location.err = 0.0;
    Car_1.motro_state[3].location.err_last = 0.0;
    Car_1.motro_state[3].location.integral = 0.0;

    Car_1.motro_state[3].location.Kp = 0.24;
    Car_1.motro_state[3].location.Ki = 0.0;
    Car_1.motro_state[3].location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro_state[3].seppd.target_val = 0.0;
    Car_1.motro_state[3].seppd.actual_val = 0.0;
    Car_1.motro_state[3].seppd.err = 0.0;
    Car_1.motro_state[3].seppd.err_last = 0.0;
    Car_1.motro_state[3].seppd.integral = 0.0;

    Car_1.motro_state[3].seppd.Kp = 1.0;
    Car_1.motro_state[3].seppd.Ki = 0.4;
    Car_1.motro_state[3].seppd.Kd = 1.5;
}

void set_pid_target(_pid *pid, float temp_val)
{
  pid->target_val = temp_val;    // 设置当前的目标值
}

float speed_pid_realize(_pid *pid, float actual_val)
{
    /*计算目标值与实际值的误差*/
    pid->err = pid->target_val - actual_val;

    if ((pid->err < 0.5f) && (pid->err > -0.5f)) //差1这么多可以吗？运行1分钟，位置差为1个轮子的周长
    {
        pid->err = 0.0f;
    }

    pid->integral += pid->err; // 误差累积

    /*积分限幅*/
    if (pid->integral >= 3000)
    {
        pid->integral = 3000;
    }
    else if (pid->integral < -3000)
    {
        pid->integral = -3000;
    }

    /*PID算法实现*/
    pid->actual_val = pid->Kp * pid->err + pid->Ki * pid->integral + pid->Kd * (pid->err - pid->err_last);

    /*误差传递*/
    pid->err_last = pid->err;

    /*返回当前实际值*/
    // printf("                        speed_pid_target:%f, speed_pid_actual:%f\r\n", pid->target_val, pid->actual_val);
    return pid->actual_val;
}


float location_pid_realize(_pid *pid, float actual_val)  //位置环光个Kp好像也可以
{
		/*计算目标值与实际值的误差*/
    pid->err=pid->target_val-actual_val;
  
//    /* 设定闭环死区 */   //外环死区可以不要 
//    if((pid->err >= -0.1) && (pid->err <= 0.1)) 
//    {
//      pid->err = 0;
//      pid->integral = 0;
//    }
    
    pid->integral += pid->err;    // 误差累积

		/*PID算法实现*/
    pid->actual_val = pid->Kp*pid->err
		                  +pid->Ki*pid->integral
		                  +pid->Kd*(pid->err-pid->err_last);
    // printf("                        pid_target:%f, pid_actual:%f\r\n", pid->target_val, pid->actual_val);
  
		/*误差传递*/
    pid->err_last=pid->err;
    
		/*返回当前实际值*/
    return pid->actual_val;
}


