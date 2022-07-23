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
 
 //test1�ύ
 
 //test branch_uart commit 
#include "main.h"
// OLED��ʾ�ź���
SemaphoreHandle_t OLED_Bin_SemaphoreHandle;
// ���ֲ�����һ�����ź���
SemaphoreHandle_t BEEP_Bin_SemaphoreHandle;
// �����¼���־��
EventGroupHandle_t Key_EventGroupHandle;
/**
 * @name: main
 * @msg: ������
 * @return {*}
 */
 u8 dmabuf[100];
  u8 dmabuf1[100];
 int pid,i;
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����ϵͳ�ж����ȼ�����4
    delay_init();                                   //��ʱ������ʼ��
    uart_init(921600);                              //��ʼ������
		//USARTx_DMA_Config();														//����ʹ�ô���1DMAģʽ 
		USART1_TX_DMA_Config();
		FIFO_DMA_INIT(&USARTD1);
    USART2_Init(115200);                            //����2��ʼ��
    usart3_init(115200);                            //����3��ʼ��
    usart5_init(115200);                            //����5��ʼ�� */
    LED_Init();                                     //��ʼ��LED
     OLED_Init();                                    // OLED��ʼ��
    KEY_Init();                                     //������ʼ��
    TB6612_Init();                                  // TB6612��ʼ��
    PID_Init();                                     //��ʼ��PID
    ADC4_Init();                                    //ADC��ʼ��
    Beep_Init();                                    //Beep��ʼ��

    Timer2_PWM_Init(3600 - 1, 0);                   //��ʼ����ʱ��2���4·20KHz��pwm
    Timer3_Encoder_Init(65535, 0);                  //��ʼ����ʱ��Ϊ������ģʽ
    Timer4_Encoder_Init(65535, 0);                  //��ʼ����ʱ��Ϊ������ģʽ
    Timer5_Encoder_Init(65535, 0);                  //��ʼ����ʱ��Ϊ������ģʽ
    Timer8_Encoder_Init(65535, 0);                  //��ʼ����ʱ��Ϊ������ģʽ

    //����ռ�ձ�
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, 0);
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
    Car_Direction(stop, 1);
    Car_Direction(stop, 2);
    Car_Direction(stop, 3);
    Car_Direction(stop, 4);
    LCD_ShowChar(0, 0, 'A', 1);
    // LCD��ʼ������������!!!!!
    //Lcd_Init();
    LCD_Clear(WHITE); //����
    BACK_COLOR=WHITE;
	POINT_COLOR=RED;	
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
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
//extern u16 ADC_Value;

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
				//DMA1_Channel4->CNDTR = 10;       		//3.����Ҫ�����������buffersize��
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
        sprintf((char*)voltage_str, "%2.2fV", output_voltage(Car_1.car_battery_voltage));
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
    while (1)
    {
        // taskENTER_CRITICAL(); //�����ٽ���

        // taskEXIT_CRITICAL();  //�˳��ٽ���
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
            printf("����\r\n");
        }
        delay_ms(100);
    }
}
