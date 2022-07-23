/*
 * @Description: timer.c ������ʱ���ͱ�������ʼ��,PWM�Ⱥ���ʵ���Լ�����ж�
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:57:55
 * @LastEditTime: 2022-07-21 13:42:14
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\ʵ��\RCT6_4��С��\HARDWARE\TIMER\timer.c
 */
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#include "timer.h"
#include "tb6612.h"
#include "usart.h"
#include "Beep.h"
/**
 * @name: Timer2_PWM_Init
 * @msg: ��ʱ��2���4·PWM 20KHz
 * PA15:CH1, PB3:CH2 PA2:CH3, PA3:CH4
 * @return {*}
 */
void Timer2_PWM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // ��ʼ��ʱ�Ӻ͸���
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    /*  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);    // ��ȫ��ӳ��
     GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //����JTAG
      */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //��ֹJTAG���ܣ���PB3��PB4��Ϊ��ͨIO��ʹ��

    // ��ʼ��IO��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // PWM-IO
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // GPIO_Mode_AF_PP
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11; // PWM-IO
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // GPIO_Mode_AF_PP
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ��ʼ����ʱ�����PWM 72M/20K=3600
    TIM_DeInit(TIM2);                                           //������ TIMx �Ĵ�������Ϊȱʡֵ
    TIM_TimeBaseStructure.TIM_Period = arr;                     //����������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //������������Ϊ TIM1 ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ������ʱ�Ӳ��ָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //ѡ���˼�����ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);             //���� TIM_TimeBaseInitStruct ��ָ���Ĳ�����ʼ�� TIMx ��ʱ�������λ

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //ѡ��ʱ��ģʽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //ѡ������Ƚ�״̬
    TIM_OCInitStructure.TIM_Pulse = 0;                            //�����˴�װ�벶��ȽϼĴ���������ֵ
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //�������

    TIM_OC1Init(TIM2, &TIM_OCInitStructure); //���� TIM2_OCInitStruct ��ָ���Ĳ�����ʼ�� TIM2ͨ�� 1
    TIM_OC2Init(TIM2, &TIM_OCInitStructure); //���� TIM2_OCInitStruct ��ָ���Ĳ�����ʼ�� TIM2ͨ�� 2
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); //���� TIM2_OCInitStruct ��ָ���Ĳ�����ʼ�� TIM2ͨ�� 3
    TIM_OC4Init(TIM2, &TIM_OCInitStructure); //���� TIM2_OCInitStruct ��ָ���Ĳ�����ʼ�� TIM2ͨ�� 4

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); //ʹ�ܻ���ʧ�� TIMx �� CCR1 �ϵ�Ԥװ�ؼĴ���
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); //ʹ�ܻ���ʧ�� TIMx �� CCR1 �ϵ�Ԥװ�ؼĴ���
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); //ʹ�ܻ���ʧ�� TIMx �� CCR1 �ϵ�Ԥװ�ؼĴ���
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); //ʹ�ܻ���ʧ�� TIMx �� CCR1 �ϵ�Ԥװ�ؼĴ���

    TIM_CtrlPWMOutputs(TIM2, ENABLE);   // MOE �����ʹ��
    TIM_ARRPreloadConfig(TIM2, ENABLE); //ʹ�ܻ���ʧ�� TIMx �� ARR �ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM2, ENABLE); //ʹ�ܻ���ʧ�� TIMx ����
}

/**
 * @name: Timer3_Encoder_Init
 * @msg: ��ʱ��3������ģʽ ���1ʹ��
 * @return {*}
 */
void Timer3_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //ʹ�ܶ�ʱ��2��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //�����趨������ʼGPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = arr;                     //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising,
                               TIM_ICPolarity_Rising); //ʹ�ñ�����ģʽ3��ģʽ3��������������˵��4��Ƶ����ϸ��Ϣ�鿴stm32f1�����ֲ�

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    // Reset counter

    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @name: Timer4_Encoder_Init
 * @msg: ��ʱ��4������ģʽ ���2ʹ��
 * @return {*}
 */
void Timer4_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //ʹ�ܶ�ʱ��4��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //�����趨������ʼ��GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = arr;                     //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //ʹ�ñ�����ģʽ3

    TIM_ICStructInit(&TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    // Reset counter
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

/**
 * @name: Timer5_Encoder_Init
 * @msg: ��ʱ��5������ģʽ ���3ʹ��
 * @return {*}
 */
void Timer5_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //ʹ�ܶ�ʱ��5��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0; //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //�����趨������ʼ��GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = arr;                     //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //ʹ�ñ�����ģʽ3

    TIM_ICStructInit(&TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    // Reset counter
    TIM_SetCounter(TIM5, 0);
    TIM_Cmd(TIM5, ENABLE);
}

/**
 * @name: Timer8_Encoder_Init
 * @msg: ��ʱ��8������ģʽ ���4ʹ��
 * @return {*}
 */
void Timer8_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  //ʹ�ܶ�ʱ��5��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);                 //�����趨������ʼ��GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = arr;                     //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //ʹ�ñ�����ģʽ3

    TIM_ICStructInit(&TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM8, TIM_FLAG_Update); //���TIM�ĸ��±�־λ
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
    // Reset counter
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM8, ENABLE);
}

/**
 * @name: Read_Encode_Num
 * @msg: ��ȡ������, ����û��
 * @param {u8} TIMx��ʱ��
 * @return {*}
 */
int Read_Encode_Num(u8 TIMx)
{
    switch (TIMx)
    {
    case 3:
        Car_1.motro1_state.encode_num = -((short)TIM3->CNT);                  //��ȡ������
        Car_1.motro1_state.total_encode_num += Car_1.motro1_state.encode_num; //�����������
        TIM3->CNT = 0;                                                        //����
        break;
    case 4:
        Car_1.motro2_state.encode_num = -((short)TIM4->CNT);                  //��ȡ������
        Car_1.motro2_state.total_encode_num += Car_1.motro2_state.encode_num; //�����������
        TIM4->CNT = 0;                                                        //����
        break;
    case 5:
        Car_1.motro3_state.encode_num = -((short)TIM5->CNT);                  //��ȡ������
        Car_1.motro3_state.total_encode_num += Car_1.motro3_state.encode_num; //�����������
        TIM5->CNT = 0;                                                        //����
        break;
    case 8:
        Car_1.motro4_state.encode_num = -((short)TIM8->CNT);                  //��ȡ������
        Car_1.motro4_state.total_encode_num += Car_1.motro4_state.encode_num; //�����������
        TIM8->CNT = 0;                                                        //����
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @name: TIM1_Init
 * @msg: ��ʼ����ʱ��1,����BEEP,��Ϊû��������PWM�������
 *       ����ֻ��ʹ���жϷ�ʽ�ı��ƽ,һ���жϴ���ʱ��Ϊ10us,���Ƶ��Ϊ100KHz
 * @param {u16} arr:װ��ֵ
 * @param {u16} psc��Ƶֵ
 * @return {*}
 */
void Timer1_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVICinitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = arr;                     //����������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //������������Ϊ TIM1 ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ������ʱ�Ӳ��ָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //ѡ���˼�����ģʽ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //���� TIM_TimeBaseInitStruct ��ָ���Ĳ�����ʼ�� TIMx ��ʱ�������λ

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);      //���жϱ�־λ
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); //|TIM_IT_Trigger
    NVICinitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVICinitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVICinitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVICinitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVICinitStructure);
    // TIM_Cmd(TIM1, ENABLE); //ʹ��TIMx����
}

extern SemaphoreHandle_t BEEP_Bin_SemaphoreHandle;
void TIM1_UP_IRQHandler(void)
{
    static u16 i;
    BaseType_t pxHigherPriorityTaskWoken;
    BaseType_t xTaskWokenByReceive = pdFALSE;
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        if ((Beep.beep_en == beep_enable || Beep.beep_en == beep_busy) && (int)Beep.pinlv >= 0)
        {
            Beep.pinlv = Beep.pinlv - 10.0;
            i++;
            // printf("i:%d, pv:%d\r\n", i, (int)Beep.pinlv);
            Beep.beep_en = beep_busy;
            if (Beep.high_time < (int)Beep.pinlv)
            {
                BEEP_HIGH;
            }
            else
            {
                BEEP_LOW;
            }
        }
        else if (Beep.jiepai > 0)
        {
            Beep.jiepai--;
            Beep.pinlv = Beep.pinlv_m;
        }
        else
        {
            BEEP_OFF;
            i = 0;
            // printf("Beep\r\n");
            if (Beep.beep_play_music == beep_enable)
                xSemaphoreGiveFromISR(BEEP_Bin_SemaphoreHandle, &pxHigherPriorityTaskWoken);
            Beep.beep_en = beep_disable;
        }
    }
    portYIELD_FROM_ISR(xTaskWokenByReceive); //�����Ҫ�Ļ�����һ�������л�
}
