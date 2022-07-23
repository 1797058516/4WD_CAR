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
 
 //test1提交
 
 //test branch_uart commit 
#include "main.h"
// OLED显示信号量
SemaphoreHandle_t OLED_Bin_SemaphoreHandle;
// 音乐播放下一节拍信号量
SemaphoreHandle_t BEEP_Bin_SemaphoreHandle;
// 按键事件标志组
EventGroupHandle_t Key_EventGroupHandle;
/**
 * @name: main
 * @msg: 主函数
 * @return {*}
 */
 u8 dmabuf[100];
  u8 dmabuf1[100];
 int pid,i;
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置系统中断优先级分组4
    delay_init();                                   //延时函数初始化
    uart_init(921600);                              //初始化串口
		//USARTx_DMA_Config();														//配置使用串口1DMA模式 
		USART1_TX_DMA_Config();
		FIFO_DMA_INIT(&USARTD1);
    USART2_Init(115200);                            //串口2初始化
    usart3_init(115200);                            //串口3初始化
    usart5_init(115200);                            //串口5初始化 */
    LED_Init();                                     //初始化LED
     OLED_Init();                                    // OLED初始化
    KEY_Init();                                     //按键初始化
    TB6612_Init();                                  // TB6612初始化
    PID_Init();                                     //初始化PID
    ADC4_Init();                                    //ADC初始化
    Beep_Init();                                    //Beep初始化

    Timer2_PWM_Init(3600 - 1, 0);                   //初始化定时器2输出4路20KHz的pwm
    Timer3_Encoder_Init(65535, 0);                  //初始化定时器为编码器模式
    Timer4_Encoder_Init(65535, 0);                  //初始化定时器为编码器模式
    Timer5_Encoder_Init(65535, 0);                  //初始化定时器为编码器模式
    Timer8_Encoder_Init(65535, 0);                  //初始化定时器为编码器模式

    //设置占空比
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, 0);
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
    Car_Direction(stop, 1);
    Car_Direction(stop, 2);
    Car_Direction(stop, 3);
    Car_Direction(stop, 4);
    LCD_ShowChar(0, 0, 'A', 1);
    // LCD初始化必须放在最后!!!!!
    //Lcd_Init();
    LCD_Clear(WHITE); //清屏
    BACK_COLOR=WHITE;
	POINT_COLOR=RED;	
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

    OLED_Bin_SemaphoreHandle = xSemaphoreCreateBinary();
    if (OLED_Bin_SemaphoreHandle == NULL)
        printf("Couldn't create OLED_Bin_SemaphoreHandle\r\n'");
    BEEP_Bin_SemaphoreHandle = xSemaphoreCreateBinary();
    if (BEEP_Bin_SemaphoreHandle == NULL)
        printf("Couldn't create BEEP_Bin_SemaphoreHandle\r\n'");
    Key_EventGroupHandle = xEventGroupCreate();
    if (Key_EventGroupHandle == NULL)
        printf("Couldn't create Key_EventGroupHandle\r\n");
    // OLED_Clear();
    // OLED_ShowString(0, 0, "init success!", 16);
    // delay_xms(300);
    // OLED_Clear();
    // main_page();
    Boot_prompt_tone();
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
//extern u16 ADC_Value;

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
        #if TEST_ENCODE
        Read_Encode_Num(3);
        printf("num1:%d  ", Car_1.motro1_state.encode_num);
        Read_Encode_Num(4);
        printf("num2:%d  ", Car_1.motro2_state.encode_num);
        Read_Encode_Num(5);
        printf("num3:%d  ", Car_1.motro3_state.encode_num);
        Read_Encode_Num(8);
        printf("num4:%d\r\n", Car_1.motro4_state.encode_num);
        #endif
        // printf("led task\r\n");
        //sound1();
       /*  temp =(float) (ADC_MAX_VOLTAGE/4096)*ADC_Value;
        temp = temp/VM_M*BATTERY_VOLTAGE; */
				//u3_printf("led task");
				
				//printf("%d\r\n",pid++);
//				 for(i=0;i<SENDBUFF_SIZE;i++)
//				{
//						SendBuff[i]	 = 'P';
//    
//				}
				//USART_DMACmd(USART1,USART_DMAReq_Tx , DISABLE);
				//DMA1_Channel4->CNDTR = 10;       		//3.设置要传输的数据量buffersize个
				//DMA_Cmd(DMA1_Channel4, ENABLE );

				for(i=0;i<100;i++)
				{
						dmabuf1[i]	 = 'Q';
    
				}
				//USART_DMACmd(USART1,USART_DMAReq_Tx , ENABLE);
				//DMA_Cmd(DMA1_Channel4, DISABLE );

				//DMA_USART1_Tx_Data(dmabuf1,99);
				//USART1_printf("AA");
				u1_printf("ackdj:%d\r\n",i);
				i=0;
				if(pid>100) pid=0;
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
        sprintf((char*)voltage_str, "%2.2fV", output_voltage(Car_1.car_battery_voltage));
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
    while (1)
    {
        // taskENTER_CRITICAL(); //进入临界区

        // taskEXIT_CRITICAL();  //退出临界区
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
    while(1)
    {
        xSemaphoreTake(BEEP_Bin_SemaphoreHandle, portMAX_DELAY);
        Beep_Voice(&Beep, 2, f[i], JP[i]*208);
        // Beep_Voice(&Beep, 30, 262, 500);
        printf("i:%d\r\n", i);
        i++;
        if(i == strlen((char*)JP)-2)
        {
            i = 0;
            Beep.beep_play_music = beep_disable;
            printf("结束\r\n");
        }
        delay_ms(100);
    }
}
