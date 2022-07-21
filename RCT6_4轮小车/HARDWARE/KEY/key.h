/*
 * @Description: key.h
 * @Author: TOTHTOT
 * @Date: 2022-07-17 10:17:02
 * @LastEditTime: 2022-07-18 19:47:25
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\ʵ��\RCT6_4��С��\HARDWARE\KEY\key.h
 */
#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define SW3 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)   //��ȡ����3
#define SW4 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)   //��ȡ����4
#define WK_UP GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) //��ȡWK_UP

#define SW3_EVENTGROUPS_BIT 1 << 0
#define SW4_EVENTGROUPS_BIT 1 << 1
#define WK_UP_EVENTGROUPS_BIT 1 << 2

#define SW3_PRESS 1 // SW3
#define SW4_PRES 2  // SW4
#define WKUP_PRES 3 // WK_UP

void KEY_Init(void);  // IO��ʼ��
u8 KEY_Scan(u8 mode); //����ɨ�躯��
#endif
