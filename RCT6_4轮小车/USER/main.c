/*
 *                                                     __----~~~~~~~~~~~------___
 *                                    .  .   ~~//====......          __--~ ~~
 *                    -.            \_|//     |||\\  ~~~~~~::::... /~
 *                 ___-==_       _-~o~  \/    |||  \\            _/~~-
 *         __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 *     _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 *   .~       .~       |   \\ -_    /  /-   /   ||      \   /
 *  /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 *  |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 *           '         ~-|      /|    |-~\~~       __--~~
 *                       |-~~-_/ |    |   ~\_   _-~            /\
 *                            /  \     \__   \/~                \__
 *                        _--~ _/ | .-~~____--~-/                  ~~==.
 *                       ((->/~   '.|||' -_|    ~~-/ ,              . _||
 *                                  -_     ~\      ~~---l__i__i__i--~~_/
 *                                  _-~-__   ~)  \--______________--~~
 *                                //.-~~~-~_--~- |-------~~~~~~~~
 *                                       //.-~~~--\
 *                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *                               神兽保佑            永无BUG
 */

/*
 * @Description: main.c
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:40:37
 * @LastEditTime: 2022-07-14 16:45:04
 * @LastEditors: TOTHTOT
 * @FilePath: \USER\main.c
 */

// test1提交

// test branch_uart commit
#include "main.h"
// OLED显示信号量
SemaphoreHandle_t OLED_Bin_SemaphoreHandle;
// 音乐播放下一节拍信号量
SemaphoreHandle_t BEEP_Bin_SemaphoreHandle;
// MPU6050读取数据信号量
SemaphoreHandle_t MPU6050_Bin_SemaphoreHandle;
// 按键事件标志组
EventGroupHandle_t Key_EventGroupHandle;
/**
 * @name: main
 * @msg: 主函数
 * @return {*}
 */

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置系统中断优先级分组4
    delay_init();                                   //延时函数初始化
    uart_init(921600);                              //初始化串口
    // USARTx_DMA_Config();														//配置使用串口1DMA模式
    USART1_TX_DMA_Config();
    USART2_Init(460800); //串口2初始化,和蓝牙通信使用模块支持的最大波特率
    usart3_init(115200); //串口3初始化
    usart5_init(115200); //串口5初始化
    LED_Init();          //初始化LED
    OLED_Init();         // OLED初始化
    KEY_Init();          //按键初始化
    TB6612_Init();       // TB6612初始化
    PID_Init();          //初始化PID
    ADC4_Init();         // ADC初始化
    Beep_Init();         // Beep初始化
    MPU_Init();          // MPU初始化

    Timer2_PWM_Init(3600 - 1, 0);  //初始化定时器2输出4路20KHz的pwm
    Timer3_Encoder_Init(65535, 0); //初始化定时器为编码器模式
    Timer4_Encoder_Init(65535, 0); //初始化定时器为编码器模式
    Timer5_Encoder_Init(65535, 0); //初始化定时器为编码器模式
    Timer8_Encoder_Init(65535, 0); //初始化定时器为编码器模式

    //设置占空比
    /*    TIM_SetCompare1(TIM2, 1000);
       TIM_SetCompare2(TIM2, 1000);
       TIM_SetCompare3(TIM2, 1000);
       TIM_SetCompare4(TIM2, 1000);
       Car_Direction(zhengzhuan, 1);
       Car_Direction(zhengzhuan, 2);
       Car_Direction(zhengzhuan, 3);
       Car_Direction(zhengzhuan, 4);
       LCD_ShowChar(0, 0, 'A', 1); */
    // LCD初始化必须放在最后!!!!!
    // Lcd_Init();
    LCD_Clear(WHITE); //清屏
    BACK_COLOR = WHITE;
    POINT_COLOR = RED;
    printf(" 初始化完成!!!\r\n");
    //创建开始任务
    xTaskCreate((TaskFunction_t)start_task,          //任务函数
                (const char *)"start_task",          //任务名称
                (uint16_t)START_STK_SIZE,            //任务堆栈大小
                (void *)NULL,                        //传递给任务函数的参数
                (UBaseType_t)START_TASK_PRIO,        //任务优先级
                (TaskHandle_t *)&StartTask_Handler); //任务句柄
    vTaskStartScheduler();                           //开启任务调度
}

/**
 * @name: start_task
 * @msg: 初始化任务
 * @param {void} *pvParameters
 * @return {*}
 */
void start_task(void *pvParameters)
{
    u8 err;
    taskENTER_CRITICAL(); //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t)led0_task,
                (const char *)"led0_task",
                (uint16_t)LED0_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED0_TASK_PRIO,
                (TaskHandle_t *)&LED0Task_Handler);
    //创建OLED任务
    xTaskCreate((TaskFunction_t)oled_task,
                (const char *)"oled_task",
                (uint16_t)OLED_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)OLED_TASK_PRIO,
                (TaskHandle_t *)&OLEDTask_Handler);
    //创建PID任务
    xTaskCreate((TaskFunction_t)pid_task,
                (const char *)"pid_task",
                (uint16_t)PID_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)PID_TASK_PRIO,
                (TaskHandle_t *)&PIDTask_Handler);
    //创建KEY任务
    xTaskCreate((TaskFunction_t)key_task,
                (const char *)"key_task",
                (uint16_t)KEY_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)KEY_TASK_PRIO,
                (TaskHandle_t *)&KEYTask_Handler);
    //创建Play_Music任务
    xTaskCreate((TaskFunction_t)play_music_task,
                (const char *)"music_task",
                (uint16_t)Play_Music_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)Play_Music_TASK_PRIO,
                (TaskHandle_t *)&Play_MusicTask_Handler);
    //创建MPU6050任务
    xTaskCreate((TaskFunction_t)MPU6050_task,
                (const char *)"music_task",
                (uint16_t)MPU6050_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)MPU6050_TASK_PRIO,
                (TaskHandle_t *)&MPU6050Task_Handler);

    OLED_Bin_SemaphoreHandle = xSemaphoreCreateBinary();
    if (OLED_Bin_SemaphoreHandle == NULL)
        printf("Couldn't create OLED_Bin_SemaphoreHandle\r\n'");
    BEEP_Bin_SemaphoreHandle = xSemaphoreCreateBinary();
    if (BEEP_Bin_SemaphoreHandle == NULL)
        printf("Couldn't create BEEP_Bin_SemaphoreHandle\r\n'");
    MPU6050_Bin_SemaphoreHandle = xSemaphoreCreateBinary();
    if (MPU6050_Bin_SemaphoreHandle == NULL)
        printf("Couldn't create MPU6050_Bin_SemaphoreHandle\r\n'");
    Key_EventGroupHandle = xEventGroupCreate();
    if (Key_EventGroupHandle == NULL)
        printf("Couldn't create Key_EventGroupHandle\r\n");
    /*     OLED_Clear();
        OLED_ShowString(0, 0, "init success!", 16);
        delay_xms(300);
        OLED_Clear();
        main_page(); */
    Boot_prompt_tone();
    /* 避免应MPU6050自己的中断而打断初始化,且在信号量没有创建完成时而释放造成的错误 */
    err = mpu_dmp_init();
    while (err != 0)
    {
        err = mpu_dmp_init();
        printf("初始化dmp error:%d\r\n", err);
    }
    MPU6050_INT_Init();
    // Car_Go(50, &Car_1);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
// extern u16 ADC_Value;

/**
 * @name: led0_task
 * @msg: LED0翻转任务
 * @return {*}
 */
void led0_task(void *pvParameters)
{
    while (1)
    {
        LED0 = ~LED0;
        /*    Read_Encode_Num(3);
           u1_printf("num1:%d  ", Car_1.motro_state[0].encode_num);
           Read_Encode_Num(4);
           u1_printf("num2:%d  ", Car_1.motro_state[1].encode_num);
           Read_Encode_Num(5);
           u1_printf("num3:%d  ", Car_1.motro_state[2].encode_num);
           Read_Encode_Num(8);
           u1_printf("num4:%d\r\n", Car_1.motro_state[3].encode_num); */
        // printf("led task\r\n");
        // sound1();
        /*  temp =(float) (ADC_MAX_VOLTAGE/4096)*ADC_Value;
         temp = temp/VM_M*BATTERY_VOLTAGE; */
        // u3_printf("led task");
        // u1_printf("ackdj:%d\r\n", 22);

        delay_ms(500);
    }
}
/**
 * @name: oled_task
 * @msg: 发送一次OLED_Bin_SemaphoreHandle信号量更新一次oled
 * @param {void} *pvParameters
 * @return {*}
 */
void oled_task(void *pvParameters)
{
    u8 voltage_str[20];
    while (1)
    {
        // xSemaphoreTake(OLED_Bin_SemaphoreHandle, portMAX_DELAY);
        // printf("refresh oled data\n\r");
        // printf("adc:%d, %f\r\n", Car_1.car_battery_voltage, output_voltage(Car_1.car_battery_voltage));
        sprintf((char *)voltage_str, "%2.2fV", output_voltage(Car_1.car_battery_voltage));
        LCD_ShowString(0, 0, voltage_str);
        // main_page_data()
        delay_ms(200);
    }
}

/**
 * @name: pid_task
 * @msg: pid计算任务每隔Car_PID_CYCLE执行一次,此任务优先级最高
 * @param {void} *pvParameters
 * @return {*}
 */
void pid_task(void *pvParameters)
{
    short Motor_Straight_Control_Num = 0; //用来处理巡线
    u8 stop_count = 0;                    // 小车停止计数值
    u8 stop_count_target = 10;            //小车停止计数目标值,随情况而定
    while (1)
    {
        if (Car_1.pid_en == 1)
        {
            u1_printf("pid task\r\n");
            // taskENTER_CRITICAL(); //进入临界区
            Read_Encode_Num(3);
            u1_printf("num1:%d  ", Car_1.motro_state[0].encode_num);
            Read_Encode_Num(4);
            u1_printf("num2:%d  ", Car_1.motro_state[1].encode_num);
            Read_Encode_Num(5);
            u1_printf("num3:%d  ", Car_1.motro_state[2].encode_num);
            Read_Encode_Num(8);
            u1_printf("num4:%d\r\n", Car_1.motro_state[3].encode_num);                                                 //获取编码器的计数值
            Car_1.motro_state[0].distance = ((Car_1.motro_state[0].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE) * Car_PI * Car_CheLunZhiJing);                          //小车在Car_PID_CYCLE时间内转动的脉冲数/一圈的脉冲数再乘以直径就是路程
            Car_1.motro_state[1].distance = ((Car_1.motro_state[1].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE )* Car_PI * Car_CheLunZhiJing); //小车在Car_PID_CYCLE时间内转动的脉冲数/一圈的脉冲数再乘以直径就是路程
            Car_1.motro_state[2].distance = ((Car_1.motro_state[2].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE) * Car_PI * Car_CheLunZhiJing); //小车在Car_PID_CYCLE时间内转动的脉冲数/一圈的脉冲数再乘以直径就是路程
            Car_1.motro_state[3].distance = ((Car_1.motro_state[3].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE) * Car_PI * Car_CheLunZhiJing); //小车在Car_PID_CYCLE时间内转动的脉冲数/一圈的脉冲数再乘以直径就是路程

            printf("1:%f, 2:%f, 3:%f, 4:%f\r\n", (float)Car_1.motro_state[0].total_encode_num, (float)Car_1.motro_state[1].total_encode_num, (float)Car_1.motro_state[2].total_encode_num, (float)Car_1.motro_state[3].total_encode_num);
            u1_printf("以走距离:%f, %f, %f, %f\r\n", Car_1.motro_state[0].distance,
                      Car_1.motro_state[1].distance, Car_1.motro_state[2].distance, Car_1.motro_state[3].distance);

            /* Car_Staright_Control()来自巡线,如果需要巡线的话取消注释 */
            // Motor_Straight_Control_Num = Car_Staright_Control();
            if (Car_1.direction == forward)
            {
                Location_Speed_Control(&Car_1);
                Car_1.motro_state[0].speed_output_value_finally = Car_1.motro_state[0].speed_output_value + Motor_Straight_Control_Num;
                Car_1.motro_state[1].speed_output_value_finally = Car_1.motro_state[1].speed_output_value - Motor_Straight_Control_Num;
                Car_1.motro_state[2].speed_output_value_finally = Car_1.motro_state[2].speed_output_value + Motor_Straight_Control_Num;
                Car_1.motro_state[3].speed_output_value_finally = Car_1.motro_state[3].speed_output_value - Motor_Straight_Control_Num;
                Motor_Output(Car_1);
                // 在非常接近目标位置时停止计算,免得浪费时纠正几毫米的误差,只需对一个电机计算就行了
                if ((Car_1.motro_state[0].distance <= Car_1.motro_state[0].target_distance + 10) && (Car_1.motro_state[0].distance >= Car_1.motro_state[0].target_distance - 10))
                {
                    stop_count++;
                    if (stop_count >= stop_count_target)
                    {
                        stop_count = 0;
                        Car_1.direction = stop; //小车切换到停止状态
                        u1_printf("以走距离:%f, %f, %f, %f\r\n", Car_1.motro_state[0].distance,
                                  Car_1.motro_state[1].distance, Car_1.motro_state[2].distance, Car_1.motro_state[3].distance);
                    }
                }
            }
            if (Car_1.direction == stop)
            {
                u1_printf("停止\r\n");
                Car_Stop(&Car_1);
                stop_count = 0;
            }
            // taskEXIT_CRITICAL(); //退出临界区
        }
        delay_ms(Car_PID_CYCLE);
    }
}

/**
 * @name: key_task
 * @msg: 按键接受任务,当按键按下时对事件进行处理
 * @param {void} *pvParameters
 * @return {*}
 */
void key_task(void *pvParameters)
{
    EventBits_t key_eventvalue = 0;
    while (1)
    {
        if (Key_EventGroupHandle != NULL)
        {
            key_eventvalue = xEventGroupWaitBits(Key_EventGroupHandle, SW3_EVENTGROUPS_BIT | SW4_EVENTGROUPS_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            if (key_eventvalue != 0)
            {
                switch (key_eventvalue)
                {
                case SW3_EVENTGROUPS_BIT:
                    printf("sw3被按下\r\n");
                    xSemaphoreGive(BEEP_Bin_SemaphoreHandle);
                    Beep.beep_play_music = beep_enable;
                    Beep_Voice(&Beep, 80, 262, 500);
                    // Show_Voice();
                    break;
                case SW4_EVENTGROUPS_BIT:
                    printf("sw4被按下\r\n");
                    break;
                default:
                    break;
                }
            }
        }
        delay_ms(40);
    }
}

/**
 * @name: play_music_task
 * @msg: 蜂鸣器播放音乐,BEEP_Bin_SemaphoreHandle
 * @param {void} *pvParameters
 * @return {*}
 */
void play_music_task(void *pvParameters)
{
    u16 i = 75;
    while (1)
    {
        xSemaphoreTake(BEEP_Bin_SemaphoreHandle, portMAX_DELAY);
        Beep_Voice(&Beep, 2, f[i], JP[i] * 208);
        // Beep_Voice(&Beep, 30, 262, 500);
        printf("i:%d\r\n", i);
        i++;
        if (i == strlen((char *)JP) - 2)
        {
            i = 0;
            Beep.beep_play_music = beep_disable;
            printf("结束\r\n");
        }
        delay_ms(100);
    }
}

/**
 * @name: MPU6050_task
 * @msg: 读取MPU6050数据任务,MPU6050_Bin_SemaphoreHandle信号量置1读取
 * @param {void} *pvParameters
 * @return {*}
 */
void MPU6050_task(void *pvParameters)
{
    float pitch, roll, yaw;    //欧拉角
    float aacx, aacy, aacz;    //加速度传感器原始数据
    short gyrox, gyroy, gyroz; //陀螺仪原始数据
    short temp;                //温度
    while (1)
    {
        xSemaphoreTake(MPU6050_Bin_SemaphoreHandle, portMAX_DELAY);
        temp = MPU_Get_Temperature();               //得到温度值
        mpu_dmp_get_data(&pitch, &roll, &yaw);      //得到欧拉角
        MPU_Get_Accelerometer(&aacx, &aacy, &aacz); //得到加速度传感器数据
        MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);  //得到陀螺仪数据
        // u1_printf("%f  %f  %f  %d  %d  %d  %fC  \r\n", pitch, roll, yaw, gyrox, gyroy, gyroz, temp / 10.0);
        delay_ms(5);
    }
}
