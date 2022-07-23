/*
 * @Description: beep.h 无源蜂鸣器
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:55:28
 * @LastEditTime: 2022-07-20 18:49:08
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\BEEP\beep.h
 */
#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"
#define BEEP_ON TIM_Cmd(TIM1, ENABLE);   //使能TIM1外设
#define BEEP_OFF TIM_Cmd(TIM1, DISABLE); //失能TIM1外设
#define BEEP_HIGH GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define BEEP_LOW GPIO_ResetBits(GPIOB, GPIO_Pin_15)

typedef enum
{
    beep_disable,
    beep_enable,
    beep_busy
} E_BEEP_STATE;

typedef struct
{
    double pinlv;         //此值等于pinlv_m,但是会在每次的TIM1中断里减10,在jiepai减1时重新赋值为pinlv_m
    double pinlv_m;       //频率假设频率4KHz则周期为0.25ms 高低电平总持续时间为250us,要对其合理分配
    u16 jiepai;           //以此频率持续多久的声音
    u16 high_time;        //高电平持续时间,所占时间越短则蜂鸣器越响
    u16 low_time;         //低电平持续时间,实际用不到这个
    E_BEEP_STATE beep_en; // beep使能,1:使能, 2:正忙,0失能
    E_BEEP_STATE beep_play_music;
    u8 volume;            //音量最大100
} S_BEEP_Handle;

extern S_BEEP_Handle Beep;
void Beep_Init(void);
void Beep_Voice(S_BEEP_Handle *b, u8 volume, float Hz, u16 jp);
void Beep_Do(u8 volume, u16 jp);
void Beep_Re(u8 volume, u16 jp);
void Beep_Mi(u8 volume, u16 jp);
void Beep_Fa(u8 volume, u16 jp);
void Beep_So(u8 volume, u16 jp);
void Beep_La(u8 volume, u16 jp);
void Beep_Si(u8 volume, u16 jp);
void Show_Voice(void);
void Boot_prompt_tone(void);

#endif /* __BEEP_H*/
