/*
 * @Description: tb6612.HARDWARE
 * @Author: TOTHTOT
 * @Date: 2022-07-18 09:34:00
 * @LastEditTime: 2022-07-24 15:20:36
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\TB6612\tb6612.h
 */
#ifndef __TB6612_H
#define __TB6612_H
#include "sys.h"
#include "pid.h"

//С���������IO�ں궨��
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

// С��������ú궨��
#define Car_CheChang 20                                                // С�����ȵ�λCM
#define Car_CheLunZhiJing 4.8                                          //С������ֱ����λCM
#define Car_CheKuan 15                                                 //С������λCM
#define Car_MAXPWM 3000 * 0.8                                          //����������PWM�İٷ�֮��ʮ
#define Car_PI 3.142                                                   //����Բ����
#define Car_MAXSPEED 120                                               //�������ת��RPM
#define Car_FREQUENCY_DOUBLE 4.0                                         //��Ƶ
#define Car_MOTOR_REDUCTION_RATIO 20                                   //���ٱ�
#define Car_MOTOR_LINE_NUM 13                                          //��Ȧ����
#define Car_MOTOR_COIL (Car_MOTOR_REDUCTION_RATIO *Car_MOTOR_LINE_NUM)   //תһȦ�������
#define Car_MOTOR_PULSE_PER_CYCLE (Car_FREQUENCY_DOUBLE *Car_MOTOR_COIL) //ÿתһȦ������������ 4*260 13*20=260
#define Car_PID_CYCLE 50                                               // PIDÿ��ִ�е�ʱ����
// С������ö��
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
// ���״̬�ṹ��
typedef struct
{
    int encode_num;                   //������
    float max_speed;                  //����ٶ�
    float distance;                   //�Լ��߹��ľ���
    float target_distance;            //Ŀ�����,���趨����ʱ����
    int total_encode_num;             //��������
    float speed_output_value;         //���PWMֵ
    float speed_output_value_finally; //����Ѱ�����������PWMֵ
    _pid seppd;                       //�ٶȻ�
    _pid location;                    //λ�û�
} S_MOTOR_STATE;

// С��״̬�ṹ��
typedef struct
{
    E_CAR_DIRECTION direction;
    S_MOTOR_STATE motro_state[4];
    u16 car_battery_voltage; //С����ص�ѹԭʼ��ѹֵδ����
    char pid_en;             // pid����ʹ��,�ڴﵽĿ���ֹͣ����PID,Ҳ���Թ���������ʵ��ֹͣ����PID
} S_CAR_STATE;

extern S_CAR_STATE Car_1;

void TB6612_Init(void);
void Car_Direction(E_CAR_DIRECTION direction, u8 motor);
void Location_Speed_Control(S_CAR_STATE *car);
void Motor_Output(S_CAR_STATE car);

void Car_Struct_Init(S_CAR_STATE *car);
void Car_Go(u32 location_cm, S_CAR_STATE *car, u16 max_speed);
void Car_Stop(S_CAR_STATE *car);

#endif /* __TB6612_H */
