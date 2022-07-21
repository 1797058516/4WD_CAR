/*
 * @Description: tb6612.c
 * @Author: TOTHTOT
 * @Date: 2022-07-18 09:33:39
 * @LastEditTime: 2022-07-21 16:11:00
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\TB6612\tb6612.c
 */

#include "tb6612.h"
#include "usart.h"
S_CAR_STATE Car_1;
/**
 * @name: TB6612_Init
 * @msg: 初始化车轮的方向控制IO口
 * @return {*}
 */
void TB6612_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                           //输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                          // IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);                                                                     //根据设定参数初始化
    GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);        // 输出低

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
}

/**
 * @name:  Car_Direction
 * @msg: 小车方向控制,方向没有调整
 * @param {E_CAR_DIRECTION} direction 控制正反转
 * @param {u8} motor    选择控制的电机
 * @return {*}
 */
void Car_Direction(E_CAR_DIRECTION direction, u8 motor)
{
    switch (motor)
    {
    case 1:
        if (direction == zhengzhuan)
        {
            M1_AIN1 = Car_IO_HIGH;
            M1_AIN2 = Car_IO_LOW; //正转
            printf("1\r\n");
        }
        else if (direction == fanzhuan)
        {
            M1_AIN1 = Car_IO_LOW;
            M1_AIN2 = Car_IO_HIGH; //反转
            printf("2\r\n");
        }
        else
        {
            M1_AIN1 = Car_IO_LOW;
            M1_AIN2 = Car_IO_LOW; //停止
        }
        break;
    case 2:
        if (direction == zhengzhuan)
        {
            M2_BIN1 = Car_IO_HIGH;
            M2_BIN2 = Car_IO_LOW; //正转

            printf("3\r\n");
        }
        else if (direction == fanzhuan)
        {
            M2_BIN1 = Car_IO_LOW;
            M2_BIN2 = Car_IO_HIGH; // 反转
            printf("4\r\n");
        }
        else
        {
            M2_BIN1 = Car_IO_LOW;
            M2_BIN2 = Car_IO_LOW; //停止
        }
        break;
    case 3:
        if (direction == zhengzhuan)
        {
            M3_AIN1 = Car_IO_HIGH;
            M3_AIN2 = Car_IO_LOW; //正转
            printf("5\r\n");
        }
        else if (direction == fanzhuan)
        {
            M3_AIN1 = Car_IO_LOW;
            M3_AIN2 = Car_IO_HIGH; //反转
        }
        else
        {
            M3_AIN1 = Car_IO_LOW;
            M3_AIN2 = Car_IO_LOW; //停止
        }
        break;
    case 4:
        if (direction == zhengzhuan)
        {
            M4_BIN1 = Car_IO_HIGH;
            M4_BIN2 = Car_IO_LOW; //正转
        }
        else if (direction == fanzhuan)
        {
            M4_BIN1 = Car_IO_LOW;
            M4_BIN2 = Car_IO_HIGH; //反转
        }
        else
        {
            M4_BIN1 = Car_IO_LOW;
            M4_BIN2 = Car_IO_LOW; //停止
        }
        break;
    }
    /* switch (direction)
    {
    case forward:
        M1_AIN1 = Car_IO_HIGH;
        M1_AIN2 = Car_IO_LOW; //正转
        M2_BIN1 = Car_IO_HIGH;
        M2_BIN2 = Car_IO_LOW; //正转
        M3_AIN1 = Car_IO_HIGH;
        M3_AIN2 = Car_IO_LOW; //正转
        M4_BIN1 = Car_IO_HIGH;
        M4_BIN2 = Car_IO_LOW; //正转
        break;
    case retreat:
        M1_AIN1 = Car_IO_LOW;
        M1_AIN2 = Car_IO_HIGH; //反转
        M2_BIN1 = Car_IO_LOW;
        M2_BIN2 = Car_IO_HIGH; //反转
        M3_AIN1 = Car_IO_LOW;
        M3_AIN2 = Car_IO_HIGH; //反转
        M4_BIN1 = Car_IO_LOW;
        M4_BIN2 = Car_IO_HIGH; //反转
        break;
    case turn_right:
        M1_AIN1 = Car_IO_HIGH;
        M1_AIN2 = Car_IO_LOW; //正转
        M2_BIN1 = Car_IO_LOW;
        M2_BIN2 = Car_IO_HIGH; //反转
        M3_AIN1 = Car_IO_HIGH;
        M3_AIN2 = Car_IO_LOW; //正转
        M4_BIN1 = Car_IO_LOW;
        M4_BIN2 = Car_IO_HIGH; //反转
        break;
    case turn_left:
        M1_AIN1 = Car_IO_LOW;
        M1_AIN2 = Car_IO_HIGH; //反转
        M2_BIN1 = Car_IO_HIGH;
        M2_BIN2 = Car_IO_LOW; //正转
        M3_AIN1 = Car_IO_LOW;
        M3_AIN2 = Car_IO_HIGH; //反转
        M4_BIN1 = Car_IO_HIGH;
        M4_BIN2 = Car_IO_LOW; //正转
        break;
    case stop:
        M1_AIN1 = Car_IO_LOW;
        M1_AIN2 = Car_IO_LOW; //停止
        M2_BIN1 = Car_IO_LOW;
        M2_BIN2 = Car_IO_LOW; //停止
        M3_AIN1 = Car_IO_LOW;
        M3_AIN2 = Car_IO_LOW; //停止
        M4_BIN1 = Car_IO_LOW;
        M4_BIN2 = Car_IO_LOW; //停止
        break;
    default:
        break;
    } */
}
/**
 * @name: Car_Speed_PID
 * @msg: 计算速度pid
 * @param {S_MOTOR_STATE} *motor
 * @return {*}
 */
float Car_Speed_PID(S_MOTOR_STATE *motor)
{
    float actual_speed = 0.0;
    float cont_value = 0.0;
    //单位时间内的脉冲数转换成速度
    actual_speed = ((float)motor->encode_num / Car_FREQUENCY_DOUBLE / Car_MOTOR_COIL) * (60.0 * 1000.0 / Car_PID_CYCLE); // rpm
    // printf("actual2_speed:%f\r\n", actual_speed);
    cont_value = speed_pid_realize(&motor->seppd, actual_speed);
    // printf("con2t_value:%f\r\n", cont_value);
    return cont_value;
}

/**
 * @name: Car_Location_PID
 * @msg: 计算位置pid
 * @param {S_MOTOR_STATE} *motor
 * @return {*}
 */
float Car_Location_PID(S_MOTOR_STATE *motor)
{
    float cont_value = 0.0;
    float actual_location = 0.0;

    actual_location = motor->total_encode_num;
    cont_value = location_pid_realize(&motor->location, actual_location);
    if (cont_value > Car_MAXSPEED) //速度限制
    {
        cont_value = Car_MAXSPEED;
    }
    else if (cont_value < -Car_MAXSPEED)
    {
        cont_value = -Car_MAXSPEED;
    }
    return cont_value;
}

/**
 * @name: Location_Speed_Control
 * @msg: 位置速度环控制
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
        Location_1_Outval = Car_Location_PID(&car->motro1_state);
        Location_2_Outval = Car_Location_PID(&car->motro2_state);
        Location_3_Outval = Car_Location_PID(&car->motro3_state);
        Location_4_Outval = Car_Location_PID(&car->motro4_state);
        // printf("Location_1_Outval:%f, Motor_1_PulseSigma:%d\r\n", Location_1_Outval, Motor_1_PulseSigma);
        // printf("Location_2_Outval:%f, Motor_2_PulseSigma:%d\r\n", Location_2_Outval, Motor_2_PulseSigma);
        // 设置目标值
        set_pid_target(&car->motro1_state.seppd, Location_1_Outval);
        set_pid_target(&car->motro2_state.seppd, Location_2_Outval);
        set_pid_target(&car->motro3_state.seppd, Location_3_Outval);
        set_pid_target(&car->motro4_state.seppd, Location_4_Outval);
    }
    location_control_count++;
    car->motro1_state.speed_output_value = Car_Speed_PID(&car->motro1_state);
    car->motro2_state.speed_output_value = Car_Speed_PID(&car->motro2_state);
    car->motro3_state.speed_output_value = Car_Speed_PID(&car->motro3_state);
    car->motro4_state.speed_output_value = Car_Speed_PID(&car->motro4_state);
}

/**
 * @name: Motor_Output
 * @msg: PWM输出
 * @param {S_CAR_STATE} car
 * @return {*}
 */
void Motor_Output(S_CAR_STATE car)
{
    if (car.motro1_state.speed_output_value >= 0) //正转
    {
        Car_Direction(zhengzhuan, 1);
    }
    else
    {
        Car_Direction(fanzhuan, 1);
        car.motro1_state.speed_output_value = -car.motro1_state.speed_output_value;
    }
    car.motro1_state.speed_output_value = car.motro1_state.speed_output_value > Car_MAXPWM ? Car_MAXPWM : car.motro1_state.speed_output_value; //是否超出最大PWM限幅

    if (car.motro2_state.speed_output_value >= 0) //正转
    {
        Car_Direction(zhengzhuan, 2);
    }
    else
    {
        Car_Direction(fanzhuan, 2);
        car.motro2_state.speed_output_value = -car.motro2_state.speed_output_value;
    }
    car.motro2_state.speed_output_value = car.motro2_state.speed_output_value > Car_MAXPWM ? Car_MAXPWM : car.motro2_state.speed_output_value; //是否超出最大PWM限幅

    if (car.motro3_state.speed_output_value >= 0) //正转
    {
        Car_Direction(zhengzhuan, 3);
    }
    else
    {
        Car_Direction(fanzhuan, 3);
        car.motro3_state.speed_output_value = -car.motro3_state.speed_output_value;
    }
    car.motro3_state.speed_output_value = car.motro3_state.speed_output_value > Car_MAXPWM ? Car_MAXPWM : car.motro3_state.speed_output_value; //是否超出最大PWM限幅

    if (car.motro4_state.speed_output_value >= 0) //正转
    {
        Car_Direction(zhengzhuan, 4);
    }
    else
    {
        Car_Direction(fanzhuan, 4);
        car.motro4_state.speed_output_value = -car.motro4_state.speed_output_value;
    }
    car.motro4_state.speed_output_value = car.motro4_state.speed_output_value > Car_MAXPWM ? Car_MAXPWM : car.motro4_state.speed_output_value; //是否超出最大PWM限幅

    TIM_SetCompare1(TIM2, car.motro1_state.speed_output_value); //设置M1的PWM
    TIM_SetCompare2(TIM2, car.motro2_state.speed_output_value); //设置M2的PWM
    TIM_SetCompare3(TIM2, car.motro3_state.speed_output_value); //设置M3的PWM
    TIM_SetCompare4(TIM2, car.motro4_state.speed_output_value); //设置M4的PWM
}
