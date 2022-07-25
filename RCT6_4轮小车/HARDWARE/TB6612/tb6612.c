/*
 * @Description: tb6612.c
 * @Author: TOTHTOT
 * @Date: 2022-07-18 09:33:39
 * @LastEditTime: 2022-07-24 15:24:04
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\TB6612\tb6612.c
 */

#include "tb6612.h"
#include "usart.h"
S_CAR_STATE Car_1;
/**
 * @name: TB6612_Init
 * @msg: ��ʼ�����ֵķ������IO��
 * @return {*}
 */
void TB6612_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                           //���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                          // IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);                                                                     //�����趨������ʼ��
    GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);        // �����

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
}

/**
 * @name:  Car_Direction
 * @msg: С���������,����û�е���
 * @param {E_CAR_DIRECTION} direction ��������ת
 * @param {u8} motor    ѡ����Ƶĵ��
 * @return {*}
 */
void Car_Direction(E_CAR_DIRECTION direction, u8 motor)
{
    switch (motor)
    {
    case 1:
        if (direction == zhengzhuan)
        {
            M1_AIN1 = Car_IO_LOW;
            M1_AIN2 = Car_IO_HIGH; //��ת
            printf("1\r\n");
        }
        else if (direction == fanzhuan)
        {
            M1_AIN1 = Car_IO_HIGH;
            M1_AIN2 = Car_IO_LOW; //��ת
            printf("2\r\n");
        }
        else
        {
            M1_AIN1 = Car_IO_LOW;
            M1_AIN2 = Car_IO_LOW; //ֹͣ
        }
        break;
    case 2:
        if (direction == zhengzhuan)
        {

            M2_BIN1 = Car_IO_LOW;
            M2_BIN2 = Car_IO_HIGH; // ��ת
            printf("3\r\n");
        }
        else if (direction == fanzhuan)
        {
            M2_BIN1 = Car_IO_HIGH;
            M2_BIN2 = Car_IO_LOW; //��ת
            printf("4\r\n");
        }
        else
        {
            M2_BIN1 = Car_IO_LOW;
            M2_BIN2 = Car_IO_LOW; //ֹͣ
        }
        break;
    case 3:
        if (direction == zhengzhuan)
        {
            M3_AIN1 = Car_IO_LOW;
            M3_AIN2 = Car_IO_HIGH; //��ת
            printf("5\r\n");
        }
        else if (direction == fanzhuan)
        {
            M3_AIN1 = Car_IO_HIGH;
            M3_AIN2 = Car_IO_LOW; //��ת
        }
        else
        {
            M3_AIN1 = Car_IO_LOW;
            M3_AIN2 = Car_IO_LOW; //ֹͣ
        }
        break;
    case 4:
        if (direction == zhengzhuan)
        {
            M4_BIN1 = Car_IO_HIGH;
            M4_BIN2 = Car_IO_LOW; //��ת
        }
        else if (direction == fanzhuan)
        {
            M4_BIN1 = Car_IO_LOW;
            M4_BIN2 = Car_IO_HIGH; //��ת
        }
        else
        {
            M4_BIN1 = Car_IO_LOW;
            M4_BIN2 = Car_IO_LOW; //ֹͣ
        }
        break;
    }
    /* switch (direction)
    {
    case forward:
        M1_AIN1 = Car_IO_HIGH;
        M1_AIN2 = Car_IO_LOW; //��ת
        M2_BIN1 = Car_IO_HIGH;
        M2_BIN2 = Car_IO_LOW; //��ת
        M3_AIN1 = Car_IO_HIGH;
        M3_AIN2 = Car_IO_LOW; //��ת
        M4_BIN1 = Car_IO_HIGH;
        M4_BIN2 = Car_IO_LOW; //��ת
        break;
    case retreat:
        M1_AIN1 = Car_IO_LOW;
        M1_AIN2 = Car_IO_HIGH; //��ת
        M2_BIN1 = Car_IO_LOW;
        M2_BIN2 = Car_IO_HIGH; //��ת
        M3_AIN1 = Car_IO_LOW;
        M3_AIN2 = Car_IO_HIGH; //��ת
        M4_BIN1 = Car_IO_LOW;
        M4_BIN2 = Car_IO_HIGH; //��ת
        break;
    case turn_right:
        M1_AIN1 = Car_IO_HIGH;
        M1_AIN2 = Car_IO_LOW; //��ת
        M2_BIN1 = Car_IO_LOW;
        M2_BIN2 = Car_IO_HIGH; //��ת
        M3_AIN1 = Car_IO_HIGH;
        M3_AIN2 = Car_IO_LOW; //��ת
        M4_BIN1 = Car_IO_LOW;
        M4_BIN2 = Car_IO_HIGH; //��ת
        break;
    case turn_left:
        M1_AIN1 = Car_IO_LOW;
        M1_AIN2 = Car_IO_HIGH; //��ת
        M2_BIN1 = Car_IO_HIGH;
        M2_BIN2 = Car_IO_LOW; //��ת
        M3_AIN1 = Car_IO_LOW;
        M3_AIN2 = Car_IO_HIGH; //��ת
        M4_BIN1 = Car_IO_HIGH;
        M4_BIN2 = Car_IO_LOW; //��ת
        break;
    case stop:
        M1_AIN1 = Car_IO_LOW;
        M1_AIN2 = Car_IO_LOW; //ֹͣ
        M2_BIN1 = Car_IO_LOW;
        M2_BIN2 = Car_IO_LOW; //ֹͣ
        M3_AIN1 = Car_IO_LOW;
        M3_AIN2 = Car_IO_LOW; //ֹͣ
        M4_BIN1 = Car_IO_LOW;
        M4_BIN2 = Car_IO_LOW; //ֹͣ
        break;
    default:
        break;
    } */
}

/**
 * @name: Car_Struct_Init
 * @msg: �ṹ��ĳ�ʼ��
 * @param {S_CAR_STATE} *car
 * @return {*}
 */
void Car_Struct_Init(S_CAR_STATE *car)
{
    u8 i;
    for (i = 0; i < 4; i++)
    {
        car->motro_state[i].distance = 0.0;
        car->motro_state[i].encode_num = 0;
        car->motro_state[i].max_speed = 0;
        car->motro_state[i].speed_output_value = 0.0f;
        car->motro_state[i].speed_output_value_finally = 0.0f;
        car->motro_state[i].target_distance = 0.0f;
        car->motro_state[i].total_encode_num = 0;
    }
    PID_Init(); // pid������λ
    car->direction = stop;
    car->pid_en = 0; // PID������ֹͣ
}

/**
 * @name: Car_Go
 * @msg: С��Ҫ�ߵľ���
 * @param {u32} location_cm:����
 * @return {*}
 */
/**
 * @name: Car_Go
 * @msg: С��Ҫ�ߵľ���
 * @param {u32} location_cm С����ʻ����
 * @param {S_CAR_STATE} *car С���ṹ��
 * @param {u16} max_speed   С������ٶ�,��λRPM,�õĻ��ǽṹ��û��
 * @return {*}
 */
void Car_Go(u32 location_cm, S_CAR_STATE *car, u16 max_speed)
{
    float car_location = 0.0;
    Car_Struct_Init(car);
    car_location = (location_cm / (Car_CheLunZhiJing * Car_PI)) * (Car_MOTOR_PULSE_PER_CYCLE); //����ת����������
    set_pid_target(&car->motro_state[0].location, car_location);
    set_pid_target(&car->motro_state[1].location, car_location);
    set_pid_target(&car->motro_state[2].location, car_location);
    set_pid_target(&car->motro_state[3].location, car_location);
    u1_printf("go staright,distance: %d, %f\r\n", location_cm, car_location);
    car->direction = forward;
    // car.
    car->motro_state[0].target_distance = location_cm;
    car->pid_en = 1;
}

/**
 * @name: Car_Stop
 * @msg: С��ֹͣ
 * @param {S_CAR_STATE} *car
 * @return {*}
 */
void Car_Stop(S_CAR_STATE *car)
{
    u8 i;
    Car_Struct_Init(car);
    for(i = 0; i < 4;i++)
	{
        Car_Direction(stop, i);
	}
    TIM_SetCompare1(TIM2, 0); //����M1��PWM
    TIM_SetCompare2(TIM2, 0); //����M2��PWM
    TIM_SetCompare3(TIM2, 0); //����M3��PWM
    TIM_SetCompare4(TIM2, 0); //����M4��PWM
    u1_printf("ֹͣ\r\n");
}
/**
 * @name: Car_Speed_PID
 * @msg: �����ٶ�pid
 * @param {S_MOTOR_STATE} *motor
 * @return {*}
 */
float Car_Speed_PID(S_MOTOR_STATE *motor)
{
    float actual_speed = 0.0;
    float cont_value = 0.0;
    //��λʱ���ڵ�������ת�����ٶ�
    actual_speed = ((float)motor->encode_num /Car_MOTOR_PULSE_PER_CYCLE) * (60.0 * 1000.0 / 50); // rpm
    u1_printf("actual2_speed:%f\r\n", actual_speed);
    cont_value = speed_pid_realize(&motor->seppd, actual_speed);
    printf("con2t_value:%f\r\n", cont_value);
    return cont_value;
}

/**
 * @name: Car_Location_PID
 * @msg: ����λ��pid
 * @param {S_MOTOR_STATE} *motor
 * @return {*}
 */
float Car_Location_PID(S_MOTOR_STATE *motor)
{
    float cont_value = 0.0;
    float actual_location = 0.0;

    actual_location = motor->total_encode_num;
    cont_value = location_pid_realize(&motor->location, actual_location);
    if (cont_value > Car_MAXSPEED) //�ٶ�����
    {
        cont_value = Car_MAXSPEED;
    }
    else if (cont_value < -Car_MAXSPEED)
    {
        cont_value = -Car_MAXSPEED;
    }
    u1_printf("cont:%f\r\n", cont_value);
    return cont_value;
}

/**
 * @name: Location_Speed_Control
 * @msg: λ���ٶȻ�����
 * @param {S_CAR_STATE *} car
 * @return {*}
 */
void Location_Speed_Control(S_CAR_STATE *car)
{
    static u8 location_control_count;
    float Location_1_Outval, Location_2_Outval, Location_3_Outval, Location_4_Outval;
    if (location_control_count >= 1)
    {
        location_control_count = 0;
        Location_1_Outval = Car_Location_PID(&car->motro_state[0]);
        Location_2_Outval = Car_Location_PID(&car->motro_state[1]);
        Location_3_Outval = Car_Location_PID(&car->motro_state[2]);
        Location_4_Outval = Car_Location_PID(&car->motro_state[3]);
        // printf("Location_1_Outval:%f, Motor_1_PulseSigma:%d\r\n", Location_1_Outval, Motor_1_PulseSigma);
        // printf("Location_2_Outval:%f, Motor_2_PulseSigma:%d\r\n", Location_2_Outval, Motor_2_PulseSigma);
        // ����Ŀ��ֵ
        set_pid_target(&car->motro_state[0].seppd, Location_1_Outval);
        set_pid_target(&car->motro_state[1].seppd, Location_2_Outval);
        set_pid_target(&car->motro_state[2].seppd, Location_3_Outval);
        set_pid_target(&car->motro_state[3].seppd, Location_4_Outval);
    }
    location_control_count++;
    car->motro_state[0].speed_output_value = Car_Speed_PID(&car->motro_state[0]);
    car->motro_state[1].speed_output_value = Car_Speed_PID(&car->motro_state[1]);
    car->motro_state[2].speed_output_value = Car_Speed_PID(&car->motro_state[2]);
    car->motro_state[3].speed_output_value = Car_Speed_PID(&car->motro_state[3]);
}

/**
 * @name: Motor_Output
 * @msg: PWM���
 * @param {S_CAR_STATE} car
 * @return {*}
 */
void Motor_Output(S_CAR_STATE car)
{
    if (car.motro_state[0].speed_output_value_finally >= 0) //��ת
    {
        Car_Direction(zhengzhuan, 1);
    }
    else
    {
        Car_Direction(fanzhuan, 1);
        car.motro_state[0].speed_output_value_finally = -car.motro_state[0].speed_output_value_finally;
    }
    car.motro_state[0].speed_output_value_finally = car.motro_state[0].speed_output_value_finally > Car_MAXPWM ? Car_MAXPWM : car.motro_state[0].speed_output_value_finally; //�Ƿ񳬳����PWM�޷�

    if (car.motro_state[1].speed_output_value_finally >= 0) //��ת
    {
        Car_Direction(zhengzhuan, 2);
    }
    else
    {
        Car_Direction(fanzhuan, 2);
        car.motro_state[1].speed_output_value_finally = -car.motro_state[1].speed_output_value_finally;
    }
    car.motro_state[1].speed_output_value_finally = car.motro_state[1].speed_output_value_finally > Car_MAXPWM ? Car_MAXPWM : car.motro_state[1].speed_output_value_finally; //�Ƿ񳬳����PWM�޷�

    if (car.motro_state[2].speed_output_value_finally >= 0) //��ת
    {
        Car_Direction(zhengzhuan, 3);
    }
    else
    {
        Car_Direction(fanzhuan, 3);
        car.motro_state[2].speed_output_value_finally = -car.motro_state[2].speed_output_value_finally;
    }
    car.motro_state[2].speed_output_value_finally = car.motro_state[2].speed_output_value_finally > Car_MAXPWM ? Car_MAXPWM : car.motro_state[2].speed_output_value_finally; //�Ƿ񳬳����PWM�޷�

    if (car.motro_state[3].speed_output_value_finally >= 0) //��ת
    {
        Car_Direction(zhengzhuan, 4);
    }
    else
    {
        Car_Direction(fanzhuan, 4);
        car.motro_state[3].speed_output_value_finally = -car.motro_state[3].speed_output_value_finally;
    }
    car.motro_state[3].speed_output_value_finally = car.motro_state[3].speed_output_value_finally > Car_MAXPWM ? Car_MAXPWM : car.motro_state[3].speed_output_value_finally; //�Ƿ񳬳����PWM�޷�

    TIM_SetCompare1(TIM2, (int)car.motro_state[0].speed_output_value_finally); //����M1��PWM
    TIM_SetCompare2(TIM2, (int)car.motro_state[1].speed_output_value_finally); //����M2��PWM
    TIM_SetCompare3(TIM2, (int)car.motro_state[2].speed_output_value_finally); //����M3��PWM
    TIM_SetCompare4(TIM2, (int)car.motro_state[3].speed_output_value_finally); //����M4��PWM
}
