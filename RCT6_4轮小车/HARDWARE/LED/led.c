/*
 * @Description: led.c
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:40:38
 * @LastEditTime: 2022-07-20 09:21:54
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\ʵ��\RCT6_4��С��\HARDWARE\LED\led.c
 */
#include "led.h"

//��ʼ��PA8��PD2Ϊ�����.��ʹ���������ڵ�ʱ��
// LED IO��ʼ��
void LED_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PA,PD�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         // LED0-->PA.8 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //�����趨������ʼ��GPIOA.8
    GPIO_SetBits(GPIOA, GPIO_Pin_8);                  // PA.8 �����
}
