/*
 * @Description: tb6612.HARDWARE
 * @Author: TOTHTOT
 * @Date: 2022-07-18 09:34:00
 * @LastEditTime: 2022-07-21 13:52:38
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\TB6612\tb6612.h
 */
#ifndef __TB6612_H
#define __TB6612_H
#include "sys.h"
#include "pid.h"

//小车方向输出IO口宏定义
#define Car_IO_HIGH 1
#define Car_IO_LOW 0
#define M1_AIN1 PCout(0)
#define M1_AIN2 PCout(1)

#define M2_BIN1 PCout(2)
#define M2_BIN2 PCout(3)

#define M3_AIN1 PCout(4)
#define M3_AIN2 PCout(5)

#define M4_BIN1 PBout(0)
#define M4_BIN2 PBout(1)

// 小车相关配置宏定义
#define Car_CheChang 20                                                      // 小车长度单位CM
#define Car_CheLunZhiJing 6.5                                                //小车车轮直径单位CM
#define Car_CheKuan 15                                                       //小车车宽单位CM
#define Car_MAXPWM 3600 * 0.8                                                //定义输出最大PWM的百分之八十
#define Car_PI 3.142                                                         //定义圆周率
#define Car_MAXSPEED 100                                                     //定义最大转速RPM
#define Car_FREQUENCY_DOUBLE 4                                               //倍频
#define Car_MOTOR_REDUCTION_RATIO 30                                         //减速比
#define Car_MOTOR_LINE_NUM 13                                                //线圈线数
#define Car_MOTOR_COIL Car_MOTOR_REDUCTION_RATIO *Car_MOTOR_LINE_NUM         //线圈数
#define Car_MOTOR_PULSE_PER_CYCLE Ca_MOTORr_FREQUENCY_DOUBLE *Car_MOTOR_COIL //每转一圈产生的脉冲数 4*390=1560 13*30=390
#define Car_PID_CYCLE 20                                                     // PID每次执行的时间间隔
// 小车方向枚举
typedef enum
{
    zhengzhuan,
    fanzhuan,
    forward,
    retreat,
    turn_right,
    turn_left,
    stop
} E_CAR_DIRECTION;
// 电机状态结构体
typedef struct
{
    int encode_num;       //脉冲数
    float speed_time;     //实时速度
    float distance;       //距离
    int total_encode_num; //总脉冲数
    float speed_output_value;   //输出PWM值
    _pid seppd;           //速度环
    _pid location;        //位置环
} S_MOTOR_STATE;

// 小车状态结构体
typedef struct
{
    E_CAR_DIRECTION direction;
    S_MOTOR_STATE motro1_state;
    S_MOTOR_STATE motro2_state;
    S_MOTOR_STATE motro3_state;
    S_MOTOR_STATE motro4_state;
    u16 car_battery_voltage;      //小车电池电压原始电压值未换算
} S_CAR_STATE;

extern S_CAR_STATE Car_1;

void TB6612_Init(void);
void Car_Direction(E_CAR_DIRECTION direction, u8 motor);

#endif /* __TB6612_H */
