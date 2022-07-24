/*
 * @Description: beep.c 无源蜂鸣器 低电平触发配合一个10us的中断使用
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:55:19
 * @LastEditTime: 2022-07-24 19:04:00
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4轮小车\HARDWARE\BEEP\beep.c
 */
#include "beep.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"
S_BEEP_Handle Beep;
//              低Si Do Re  Mi  Fa So  La  Si ?高Do?高Re?高Mi?高Fa?高So 无
uc16 tone[] = {247, 262, 294, 330, 349, 392, 440, 294, 523, 587, 659, 698, 784, 1000};
/**
 * @name: Beep_Init
 * @msg: 初始化无源蜂鸣器低电平触发
 * @return {*}
 */
void Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PA,PD端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;        // BEEP-->PB.15 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);            //根据设定参数初始化GPIOB.15
    GPIO_SetBits(GPIOB, GPIO_Pin_15);                 // PB.15 输出高
    Timer1_Init(10 - 1, 72 - 1);                      //初始化定时器1,10us溢出一次,最大频率100KHz
}

/**
 * @name: Beep_Voice
 * @msg: 发出一定频率的声音
 * @param {S_BEEP_Handle} *b beep的结构体指针
 * @param {u8} volume 音量
 * @param {float} Hz 频率
 * @param {u16} jp 节拍,声音持续时间单位ms
 * @return {*}
 */
void Beep_Voice(S_BEEP_Handle *b, u8 volume, float Hz, u16 jp)
{
    u16 h_time;

    b->beep_en = beep_enable;
    b->pinlv = (float)1.0 / Hz * 1000000.0;   //转换成us级
    h_time = b->pinlv * (100 - volume) / 100; //根据设置的音量设置高电平持续时间越小声音越大
    b->high_time = h_time;
    b->pinlv_m = b->pinlv;
    b->jiepai = (int)((jp * 1000) / (1 / Hz * 1000000));
    // printf("pl:%f\r\n", b->pinlv);
    printf("vo:%d, jp:%d\r\n", h_time, (int)((jp * 1000) / (1 / Hz * 1000000)));
    BEEP_ON; //使能定时器
}

/**
 * @name: Beep_Do
 * @msg: 发出Do音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_Do(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 882, 500);
}

/**
 * @name: Beep_Re
 * @msg: 发出Re音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_Re(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 990, 500);
}

/**
 * @name: Beep_Mi
 * @msg: 发出Mi音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_Mi(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 525, 500);
}

/**
 * @name: Beep_Fa
 * @msg: 发出Fa音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_Fa(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 589, 500);
}

/**
 * @name: Beep_So
 * @msg: 发出So音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_So(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 661, 500);
}

/**
 * @name: Beep_La
 * @msg: 发出La音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_La(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 700, 500);
}

/**
 * @name: Beep_Si
 * @msg: 发出Si音
 * @param {u8} volume 音量
 * @param {u16} jp 持续时间
 * @return {*}
 */
void Beep_Si(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 786, 500);
}

/**
 * @name: Show_Voice
 * @msg: 输出七个音符
 * @return {*}
 */
void Show_Voice(void)
{
    Beep_Do(80, 500);
    delay_ms(500);
    Beep_Re(80, 500);
    delay_ms(500);
    Beep_Mi(80, 500);
    delay_ms(500);
    Beep_Fa(80, 500);
    delay_ms(500);
    Beep_So(80, 500);
    delay_ms(500);
    Beep_La(80, 500);
    delay_ms(500);
    Beep_Si(80, 500);
    delay_ms(500);
}

/**
 * @name: Boot_prompt_tone
 * @msg: 开机提示音
 * @return {*}
 */
void Boot_prompt_tone(void)
{
    Beep_Voice(&Beep, 70, 4000, 300);
}
