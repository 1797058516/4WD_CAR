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
 *                               ���ޱ���            ����BUG
 */

/*
 * @Description: main.c
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:40:37
 * @LastEditTime: 2022-07-14 16:45:04
 * @LastEditors: TOTHTOT
 * @FilePath: \USER\main.c
 */

// test1�ύ

// test branch_uart commit
#include "main.h"
// OLED��ʾ�ź���
SemaphoreHandle_t OLED_Bin_SemaphoreHandle;
// ���ֲ�����һ�����ź���
SemaphoreHandle_t BEEP_Bin_SemaphoreHandle;
// MPU6050��ȡ�����ź���
SemaphoreHandle_t MPU6050_Bin_SemaphoreHandle;
// �����¼���־��
EventGroupHandle_t Key_EventGroupHandle;
/**
 * @name: main
 * @msg: ������
 * @return {*}
 */

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����ϵͳ�ж����ȼ�����4
    delay_init();                                   //��ʱ������ʼ��
    uart_init(921600);                              //��ʼ������
    // USARTx_DMA_Config();														//����ʹ�ô���1DMAģʽ
    USART1_TX_DMA_Config();
    USART2_Init(460800); //����2��ʼ��,������ͨ��ʹ��ģ��֧�ֵ��������
    usart3_init(115200); //����3��ʼ��
    usart5_init(115200); //����5��ʼ��
    LED_Init();          //��ʼ��LED
    OLED_Init();         // OLED��ʼ��
    KEY_Init();          //������ʼ��
    TB6612_Init();       // TB6612��ʼ��
    PID_Init();          //��ʼ��PID
    ADC4_Init();         // ADC��ʼ��
    Beep_Init();         // Beep��ʼ��
    MPU_Init();          // MPU��ʼ��

    Timer2_PWM_Init(3600 - 1, 0);  //��ʼ����ʱ��2���4·20KHz��pwm
    Timer3_Encoder_Init(65535, 0); //��ʼ����ʱ��Ϊ������ģʽ
    Timer4_Encoder_Init(65535, 0); //��ʼ����ʱ��Ϊ������ģʽ
    Timer5_Encoder_Init(65535, 0); //��ʼ����ʱ��Ϊ������ģʽ
    Timer8_Encoder_Init(65535, 0); //��ʼ����ʱ��Ϊ������ģʽ

    //����ռ�ձ�
    /*    TIM_SetCompare1(TIM2, 1000);
       TIM_SetCompare2(TIM2, 1000);
       TIM_SetCompare3(TIM2, 1000);
       TIM_SetCompare4(TIM2, 1000);
       Car_Direction(zhengzhuan, 1);
       Car_Direction(zhengzhuan, 2);
       Car_Direction(zhengzhuan, 3);
       Car_Direction(zhengzhuan, 4);
       LCD_ShowChar(0, 0, 'A', 1); */
    // LCD��ʼ������������!!!!!
    // Lcd_Init();
    LCD_Clear(WHITE); //����
    BACK_COLOR = WHITE;
    POINT_COLOR = RED;
    printf(" ��ʼ�����!!!\r\n");
    //������ʼ����
    xTaskCreate((TaskFunction_t)start_task,          //������
                (const char *)"start_task",          //��������
                (uint16_t)START_STK_SIZE,            //�����ջ��С
                (void *)NULL,                        //���ݸ��������Ĳ���
                (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t *)&StartTask_Handler); //������
    vTaskStartScheduler();                           //�����������
}

/**
 * @name: start_task
 * @msg: ��ʼ������
 * @param {void} *pvParameters
 * @return {*}
 */
void start_task(void *pvParameters)
{
    u8 err;
    taskENTER_CRITICAL(); //�����ٽ���
    //����LED0����
    xTaskCreate((TaskFunction_t)led0_task,
                (const char *)"led0_task",
                (uint16_t)LED0_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED0_TASK_PRIO,
                (TaskHandle_t *)&LED0Task_Handler);
    //����OLED����
    xTaskCreate((TaskFunction_t)oled_task,
                (const char *)"oled_task",
                (uint16_t)OLED_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)OLED_TASK_PRIO,
                (TaskHandle_t *)&OLEDTask_Handler);
    //����PID����
    xTaskCreate((TaskFunction_t)pid_task,
                (const char *)"pid_task",
                (uint16_t)PID_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)PID_TASK_PRIO,
                (TaskHandle_t *)&PIDTask_Handler);
    //����KEY����
    xTaskCreate((TaskFunction_t)key_task,
                (const char *)"key_task",
                (uint16_t)KEY_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)KEY_TASK_PRIO,
                (TaskHandle_t *)&KEYTask_Handler);
    //����Play_Music����
    xTaskCreate((TaskFunction_t)play_music_task,
                (const char *)"music_task",
                (uint16_t)Play_Music_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)Play_Music_TASK_PRIO,
                (TaskHandle_t *)&Play_MusicTask_Handler);
    //����MPU6050����
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
    /* ����ӦMPU6050�Լ����ж϶���ϳ�ʼ��,�����ź���û�д������ʱ���ͷ���ɵĴ��� */
    err = mpu_dmp_init();
    while (err != 0)
    {
        err = mpu_dmp_init();
        printf("��ʼ��dmp error:%d\r\n", err);
    }
    MPU6050_INT_Init();
    // Car_Go(50, &Car_1);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
// extern u16 ADC_Value;

/**
 * @name: led0_task
 * @msg: LED0��ת����
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
 * @msg: ����һ��OLED_Bin_SemaphoreHandle�ź�������һ��oled
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
 * @msg: pid��������ÿ��Car_PID_CYCLEִ��һ��,���������ȼ����
 * @param {void} *pvParameters
 * @return {*}
 */
void pid_task(void *pvParameters)
{
    short Motor_Straight_Control_Num = 0; //��������Ѳ��
    u8 stop_count = 0;                    // С��ֹͣ����ֵ
    u8 stop_count_target = 10;            //С��ֹͣ����Ŀ��ֵ,���������
    while (1)
    {
        if (Car_1.pid_en == 1)
        {
            u1_printf("pid task\r\n");
            // taskENTER_CRITICAL(); //�����ٽ���
            Read_Encode_Num(3);
            u1_printf("num1:%d  ", Car_1.motro_state[0].encode_num);
            Read_Encode_Num(4);
            u1_printf("num2:%d  ", Car_1.motro_state[1].encode_num);
            Read_Encode_Num(5);
            u1_printf("num3:%d  ", Car_1.motro_state[2].encode_num);
            Read_Encode_Num(8);
            u1_printf("num4:%d\r\n", Car_1.motro_state[3].encode_num);                                                 //��ȡ�������ļ���ֵ
            Car_1.motro_state[0].distance = ((Car_1.motro_state[0].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE) * Car_PI * Car_CheLunZhiJing);                          //С����Car_PID_CYCLEʱ����ת����������/һȦ���������ٳ���ֱ������·��
            Car_1.motro_state[1].distance = ((Car_1.motro_state[1].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE )* Car_PI * Car_CheLunZhiJing); //С����Car_PID_CYCLEʱ����ת����������/һȦ���������ٳ���ֱ������·��
            Car_1.motro_state[2].distance = ((Car_1.motro_state[2].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE) * Car_PI * Car_CheLunZhiJing); //С����Car_PID_CYCLEʱ����ת����������/һȦ���������ٳ���ֱ������·��
            Car_1.motro_state[3].distance = ((Car_1.motro_state[3].total_encode_num / Car_MOTOR_PULSE_PER_CYCLE) * Car_PI * Car_CheLunZhiJing); //С����Car_PID_CYCLEʱ����ת����������/һȦ���������ٳ���ֱ������·��

            printf("1:%f, 2:%f, 3:%f, 4:%f\r\n", (float)Car_1.motro_state[0].total_encode_num, (float)Car_1.motro_state[1].total_encode_num, (float)Car_1.motro_state[2].total_encode_num, (float)Car_1.motro_state[3].total_encode_num);
            u1_printf("���߾���:%f, %f, %f, %f\r\n", Car_1.motro_state[0].distance,
                      Car_1.motro_state[1].distance, Car_1.motro_state[2].distance, Car_1.motro_state[3].distance);

            /* Car_Staright_Control()����Ѳ��,�����ҪѲ�ߵĻ�ȡ��ע�� */
            // Motor_Straight_Control_Num = Car_Staright_Control();
            if (Car_1.direction == forward)
            {
                Location_Speed_Control(&Car_1);
                Car_1.motro_state[0].speed_output_value_finally = Car_1.motro_state[0].speed_output_value + Motor_Straight_Control_Num;
                Car_1.motro_state[1].speed_output_value_finally = Car_1.motro_state[1].speed_output_value - Motor_Straight_Control_Num;
                Car_1.motro_state[2].speed_output_value_finally = Car_1.motro_state[2].speed_output_value + Motor_Straight_Control_Num;
                Car_1.motro_state[3].speed_output_value_finally = Car_1.motro_state[3].speed_output_value - Motor_Straight_Control_Num;
                Motor_Output(Car_1);
                // �ڷǳ��ӽ�Ŀ��λ��ʱֹͣ����,����˷�ʱ���������׵����,ֻ���һ��������������
                if ((Car_1.motro_state[0].distance <= Car_1.motro_state[0].target_distance + 10) && (Car_1.motro_state[0].distance >= Car_1.motro_state[0].target_distance - 10))
                {
                    stop_count++;
                    if (stop_count >= stop_count_target)
                    {
                        stop_count = 0;
                        Car_1.direction = stop; //С���л���ֹͣ״̬
                        u1_printf("���߾���:%f, %f, %f, %f\r\n", Car_1.motro_state[0].distance,
                                  Car_1.motro_state[1].distance, Car_1.motro_state[2].distance, Car_1.motro_state[3].distance);
                    }
                }
            }
            if (Car_1.direction == stop)
            {
                u1_printf("ֹͣ\r\n");
                Car_Stop(&Car_1);
                stop_count = 0;
            }
            // taskEXIT_CRITICAL(); //�˳��ٽ���
        }
        delay_ms(Car_PID_CYCLE);
    }
}

/**
 * @name: key_task
 * @msg: ������������,����������ʱ���¼����д���
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
                    printf("sw3������\r\n");
                    xSemaphoreGive(BEEP_Bin_SemaphoreHandle);
                    Beep.beep_play_music = beep_enable;
                    Beep_Voice(&Beep, 80, 262, 500);
                    // Show_Voice();
                    break;
                case SW4_EVENTGROUPS_BIT:
                    printf("sw4������\r\n");
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
 * @msg: ��������������,BEEP_Bin_SemaphoreHandle
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
            printf("����\r\n");
        }
        delay_ms(100);
    }
}

/**
 * @name: MPU6050_task
 * @msg: ��ȡMPU6050��������,MPU6050_Bin_SemaphoreHandle�ź�����1��ȡ
 * @param {void} *pvParameters
 * @return {*}
 */
void MPU6050_task(void *pvParameters)
{
    float pitch, roll, yaw;    //ŷ����
    float aacx, aacy, aacz;    //���ٶȴ�����ԭʼ����
    short gyrox, gyroy, gyroz; //������ԭʼ����
    short temp;                //�¶�
    while (1)
    {
        xSemaphoreTake(MPU6050_Bin_SemaphoreHandle, portMAX_DELAY);
        temp = MPU_Get_Temperature();               //�õ��¶�ֵ
        mpu_dmp_get_data(&pitch, &roll, &yaw);      //�õ�ŷ����
        MPU_Get_Accelerometer(&aacx, &aacy, &aacz); //�õ����ٶȴ���������
        MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);  //�õ�����������
        // u1_printf("%f  %f  %f  %d  %d  %d  %fC  \r\n", pitch, roll, yaw, gyrox, gyroy, gyroz, temp / 10.0);
        delay_ms(5);
    }
}
