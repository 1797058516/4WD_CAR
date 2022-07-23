/*
 * @Description:key.c
 * @Author: TOTHTOT
 * @Date: 2022-07-17 10:17:02
 * @LastEditTime: 2022-07-18 19:54:44
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\ʵ��\RCT6_4��С��\HARDWARE\KEY\key.c
 */
#include "key.h"
#include "delay.h"
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

//������ʼ������
// PA��PC5 ���ó�����
void KEY_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE); //ʹ��PORTA,PORTCʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                         //�ⲿ�жϣ���Ҫʹ��AFIOʱ��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                     //�ر�jtag��ʹ��SWD��������SWDģʽ����
#if 0
    // PA0����ʱ��5����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
#endif
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;     // PC9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
    GPIO_Init(GPIOC, &GPIO_InitStructure);        //��ʼ��GPIOC5

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // PC8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��GPIOC.8

    // �����ж���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    // �����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x08; //��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                              //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
//����������
//���ذ���ֵ
// mode:0,��֧��������;1,֧��������;
//����ֵ��
// 0��û���κΰ�������
// SW3_PRES��SW3����
// SW4_PRES��SW4����
// WKUP_PRES��WK_UP����
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1; //�������ɿ���־
    if (mode)
        key_up = 1; //֧������
    if (key_up && (SW3 == 0 || SW4 == 0 || WK_UP == 1))
    {
        delay_ms(10); //ȥ����
        key_up = 0;
        if (SW3 == 0)
            return SW3_PRESS;
        else if (SW4 == 0)
            return SW4_PRES;
        else if (WK_UP == 1)
            return WKUP_PRES;
    }
    else if (SW3 == 1 && SW4 == 1 && WK_UP == 0)
        key_up = 1;
    return 0; // �ް�������
}
extern EventGroupHandle_t Key_EventGroupHandle;
void EXTI9_5_IRQHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken;
    delay_xms(10); //����
    if (SW3 == 0)
    {
        xEventGroupSetBitsFromISR(Key_EventGroupHandle, SW3_EVENTGROUPS_BIT, &pxHigherPriorityTaskWoken);
    }
    if (SW4 == 0)
    {
        xEventGroupSetBitsFromISR(Key_EventGroupHandle, SW4_EVENTGROUPS_BIT, &pxHigherPriorityTaskWoken);
    }
    EXTI_ClearITPendingBit(EXTI_Line8 | EXTI_Line9); //���LINE9��8�ϵ��жϱ�־λ
}
