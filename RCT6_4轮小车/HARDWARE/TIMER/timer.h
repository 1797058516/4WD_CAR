/*
 * @Description: timer.h 头文件包含定时器,编码器,PWM等相关函数的声明
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:59:05
 * @LastEditTime: 2022-07-20 17:38:18
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\TIMER\timer.h
 */
#ifndef __TIMER_H
#define __TIMER_H

/*SYSTEM*/
#include "sys.h"



void Timer1_Init(u16 arr, u16 psc);
void Timer2_PWM_Init(u16 arr, u16 psc);
void Timer3_Encoder_Init(u16 arr, u16 psc);
void Timer4_Encoder_Init(u16 arr, u16 psc);
void Timer5_Encoder_Init(u16 arr, u16 psc);
void Timer8_Encoder_Init(u16 arr, u16 psc);
int Read_Encode_Num(u8 TIMx);



#endif

