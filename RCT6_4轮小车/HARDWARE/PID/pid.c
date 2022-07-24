/*
 * @Author: TOTHTOT
 * @Date: 2022-04-04 20:15:40
 * @LastEditTime: 2022-07-24 14:15:23
 * @LastEditors: TOTHTOT
 * @Description: ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\PID\pid.c
 */
#include "pid.h"
#include "tb6612.h"

// _pid Pid_Speed1, Car_1.motro_state[1].seppd, Car_1.motro_state[1].seppd, Car_1.motro_state[1].seppd;
// _pid Car_1.motro_state[0].location, Car_1.motro_state[1].location, Car_1.motro_state[2].location, Car_1.motro_state[3].location;

void PID_Init(void)
{
    // _pid *speed_pid, _pid *location_pid
    /* λ����س�ʼ������ */
    Car_1.motro_state[0].location.target_val = 0.0;
    Car_1.motro_state[0].location.actual_val = 0.0;
    Car_1.motro_state[0].location.err = 0.0;
    Car_1.motro_state[0].location.err_last = 0.0;
    Car_1.motro_state[0].location.integral = 0.0;

    Car_1.motro_state[0].location.Kp = 0.24;
    Car_1.motro_state[0].location.Ki = 0.0;
    Car_1.motro_state[0].location.Kd = 0.0;

    /* �ٶ���س�ʼ������ */
    Car_1.motro_state[0].seppd.target_val = 0.0;
    Car_1.motro_state[0].seppd.actual_val = 0.0;
    Car_1.motro_state[0].seppd.err = 0.0;
    Car_1.motro_state[0].seppd.err_last = 0.0;
    Car_1.motro_state[0].seppd.integral = 0.0;

    Car_1.motro_state[0].seppd.Kp = 1.0;
    Car_1.motro_state[0].seppd.Ki = 0.4;
    Car_1.motro_state[0].seppd.Kd = 1.5;

    /* λ����س�ʼ������ */
    Car_1.motro_state[1].location.target_val = 0.0;
    Car_1.motro_state[1].location.actual_val = 0.0;
    Car_1.motro_state[1].location.err = 0.0;
    Car_1.motro_state[1].location.err_last = 0.0;
    Car_1.motro_state[1].location.integral = 0.0;

    Car_1.motro_state[1].location.Kp = 0.24;
    Car_1.motro_state[1].location.Ki = 0.0;
    Car_1.motro_state[1].location.Kd = 0.0;

    /* �ٶ���س�ʼ������ */
    Car_1.motro_state[1].seppd.target_val = 0.0;
    Car_1.motro_state[1].seppd.actual_val = 0.0;
    Car_1.motro_state[1].seppd.err = 0.0;
    Car_1.motro_state[1].seppd.err_last = 0.0;
    Car_1.motro_state[1].seppd.integral = 0.0;

    Car_1.motro_state[1].seppd.Kp = 1.0;
    Car_1.motro_state[1].seppd.Ki = 0.4;
    Car_1.motro_state[1].seppd.Kd = 1.5;
    
    /* λ����س�ʼ������ */
    Car_1.motro_state[2].location.target_val = 0.0;
    Car_1.motro_state[2].location.actual_val = 0.0;
    Car_1.motro_state[2].location.err = 0.0;
    Car_1.motro_state[2].location.err_last = 0.0;
    Car_1.motro_state[2].location.integral = 0.0;

    Car_1.motro_state[2].location.Kp = 0.24;
    Car_1.motro_state[2].location.Ki = 0.0;
    Car_1.motro_state[2].location.Kd = 0.0;

    /* �ٶ���س�ʼ������ */
    Car_1.motro_state[2].seppd.target_val = 0.0;
    Car_1.motro_state[2].seppd.actual_val = 0.0;
    Car_1.motro_state[2].seppd.err = 0.0;
    Car_1.motro_state[2].seppd.err_last = 0.0;
    Car_1.motro_state[2].seppd.integral = 0.0;

    Car_1.motro_state[2].seppd.Kp = 1.0;
    Car_1.motro_state[2].seppd.Ki = 0.4;
    Car_1.motro_state[2].seppd.Kd = 1.5;

    /* λ����س�ʼ������ */
    Car_1.motro_state[3].location.target_val = 0.0;
    Car_1.motro_state[3].location.actual_val = 0.0;
    Car_1.motro_state[3].location.err = 0.0;
    Car_1.motro_state[3].location.err_last = 0.0;
    Car_1.motro_state[3].location.integral = 0.0;

    Car_1.motro_state[3].location.Kp = 0.24;
    Car_1.motro_state[3].location.Ki = 0.0;
    Car_1.motro_state[3].location.Kd = 0.0;

    /* �ٶ���س�ʼ������ */
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
  pid->target_val = temp_val;    // ���õ�ǰ��Ŀ��ֵ
}

float speed_pid_realize(_pid *pid, float actual_val)
{
    /*����Ŀ��ֵ��ʵ��ֵ�����*/
    pid->err = pid->target_val - actual_val;

    if ((pid->err < 0.5f) && (pid->err > -0.5f)) //��1��ô�����������1���ӣ�λ�ò�Ϊ1�����ӵ��ܳ�
    {
        pid->err = 0.0f;
    }

    pid->integral += pid->err; // ����ۻ�

    /*�����޷�*/
    if (pid->integral >= 3000)
    {
        pid->integral = 3000;
    }
    else if (pid->integral < -3000)
    {
        pid->integral = -3000;
    }

    /*PID�㷨ʵ��*/
    pid->actual_val = pid->Kp * pid->err + pid->Ki * pid->integral + pid->Kd * (pid->err - pid->err_last);

    /*����*/
    pid->err_last = pid->err;

    /*���ص�ǰʵ��ֵ*/
    // printf("                        speed_pid_target:%f, speed_pid_actual:%f\r\n", pid->target_val, pid->actual_val);
    return pid->actual_val;
}


float location_pid_realize(_pid *pid, float actual_val)  //λ�û����Kp����Ҳ����
{
		/*����Ŀ��ֵ��ʵ��ֵ�����*/
    pid->err=pid->target_val-actual_val;
  
//    /* �趨�ջ����� */   //�⻷�������Բ�Ҫ 
//    if((pid->err >= -0.1) && (pid->err <= 0.1)) 
//    {
//      pid->err = 0;
//      pid->integral = 0;
//    }
    
    pid->integral += pid->err;    // ����ۻ�

		/*PID�㷨ʵ��*/
    pid->actual_val = pid->Kp*pid->err
		                  +pid->Ki*pid->integral
		                  +pid->Kd*(pid->err-pid->err_last);
    // printf("                        pid_target:%f, pid_actual:%f\r\n", pid->target_val, pid->actual_val);
  
		/*����*/
    pid->err_last=pid->err;
    
		/*���ص�ǰʵ��ֵ*/
    return pid->actual_val;
}


