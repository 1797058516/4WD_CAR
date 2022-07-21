/*
 * @Author: TOTHTOT
 * @Date: 2022-04-04 20:15:40
 * @LastEditTime: 2022-07-18 16:58:11
 * @LastEditors: TOTHTOT
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\PID\pid.c
 */
#include "pid.h"
#include "tb6612.h"

// _pid Pid_Speed1, Car_1.motro2_state.seppd, Car_1.motro2_state.seppd, Car_1.motro2_state.seppd;
// _pid Car_1.motro1_state.location, Car_1.motro2_state.location, Car_1.motro3_state.location, Car_1.motro4_state.location;

void PID_Init(void)
{
    // _pid *speed_pid, _pid *location_pid
    /* 位置相关初始化参数 */
    Car_1.motro1_state.location.target_val = 0.0;
    Car_1.motro1_state.location.actual_val = 0.0;
    Car_1.motro1_state.location.err = 0.0;
    Car_1.motro1_state.location.err_last = 0.0;
    Car_1.motro1_state.location.integral = 0.0;

    Car_1.motro1_state.location.Kp = 0.24;
    Car_1.motro1_state.location.Ki = 0.0;
    Car_1.motro1_state.location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro1_state.seppd.target_val = 0.0;
    Car_1.motro1_state.seppd.actual_val = 0.0;
    Car_1.motro1_state.seppd.err = 0.0;
    Car_1.motro1_state.seppd.err_last = 0.0;
    Car_1.motro1_state.seppd.integral = 0.0;

    Car_1.motro1_state.seppd.Kp = 1.0;
    Car_1.motro1_state.seppd.Ki = 0.4;
    Car_1.motro1_state.seppd.Kd = 1.5;

    /* 位置相关初始化参数 */
    Car_1.motro2_state.location.target_val = 0.0;
    Car_1.motro2_state.location.actual_val = 0.0;
    Car_1.motro2_state.location.err = 0.0;
    Car_1.motro2_state.location.err_last = 0.0;
    Car_1.motro2_state.location.integral = 0.0;

    Car_1.motro2_state.location.Kp = 0.24;
    Car_1.motro2_state.location.Ki = 0.0;
    Car_1.motro2_state.location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro2_state.seppd.target_val = 0.0;
    Car_1.motro2_state.seppd.actual_val = 0.0;
    Car_1.motro2_state.seppd.err = 0.0;
    Car_1.motro2_state.seppd.err_last = 0.0;
    Car_1.motro2_state.seppd.integral = 0.0;

    Car_1.motro2_state.seppd.Kp = 1.0;
    Car_1.motro2_state.seppd.Ki = 0.4;
    Car_1.motro2_state.seppd.Kd = 1.5;
    
    /* 位置相关初始化参数 */
    Car_1.motro3_state.location.target_val = 0.0;
    Car_1.motro3_state.location.actual_val = 0.0;
    Car_1.motro3_state.location.err = 0.0;
    Car_1.motro3_state.location.err_last = 0.0;
    Car_1.motro3_state.location.integral = 0.0;

    Car_1.motro3_state.location.Kp = 0.24;
    Car_1.motro3_state.location.Ki = 0.0;
    Car_1.motro3_state.location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro3_state.seppd.target_val = 0.0;
    Car_1.motro3_state.seppd.actual_val = 0.0;
    Car_1.motro3_state.seppd.err = 0.0;
    Car_1.motro3_state.seppd.err_last = 0.0;
    Car_1.motro3_state.seppd.integral = 0.0;

    Car_1.motro3_state.seppd.Kp = 1.0;
    Car_1.motro3_state.seppd.Ki = 0.4;
    Car_1.motro3_state.seppd.Kd = 1.5;

    /* 位置相关初始化参数 */
    Car_1.motro4_state.location.target_val = 0.0;
    Car_1.motro4_state.location.actual_val = 0.0;
    Car_1.motro4_state.location.err = 0.0;
    Car_1.motro4_state.location.err_last = 0.0;
    Car_1.motro4_state.location.integral = 0.0;

    Car_1.motro4_state.location.Kp = 0.24;
    Car_1.motro4_state.location.Ki = 0.0;
    Car_1.motro4_state.location.Kd = 0.0;

    /* 速度相关初始化参数 */
    Car_1.motro4_state.seppd.target_val = 0.0;
    Car_1.motro4_state.seppd.actual_val = 0.0;
    Car_1.motro4_state.seppd.err = 0.0;
    Car_1.motro4_state.seppd.err_last = 0.0;
    Car_1.motro4_state.seppd.integral = 0.0;

    Car_1.motro4_state.seppd.Kp = 1.0;
    Car_1.motro4_state.seppd.Ki = 0.4;
    Car_1.motro4_state.seppd.Kd = 1.5;
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
    if (pid->integral >= 5000)
    {
        pid->integral = 5000;
    }
    else if (pid->integral < -5000)
    {
        pid->integral = -5000;
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


