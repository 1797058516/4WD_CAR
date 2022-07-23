/*
 * @Description:key.c
 * @Author: TOTHTOT
 * @Date: 2022-07-17 10:17:02
 * @LastEditTime: 2022-07-18 19:54:44
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\KEY\key.c
 */
#include "key.h"
#include "delay.h"
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

//按键初始化函数
// PA和PC5 设置成输入
void KEY_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE); //使能PORTA,PORTC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                         //外部中断，需要使能AFIO时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                     //关闭jtag，使能SWD，可以用SWD模式调试
#if 0
    // PA0被定时器5用了
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
#endif
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;     // PC9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);        //初始化GPIOC5

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // PC8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化GPIOC.8

    // 配置中断线
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           //使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x08; //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                              //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}
//按键处理函数
//返回按键值
// mode:0,不支持连续按;1,支持连续按;
//返回值：
// 0，没有任何按键按下
// SW3_PRES，SW3按下
// SW4_PRES，SW4按下
// WKUP_PRES，WK_UP按下
//注意此函数有响应优先级,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1; //按键按松开标志
    if (mode)
        key_up = 1; //支持连按
    if (key_up && (SW3 == 0 || SW4 == 0 || WK_UP == 1))
    {
        delay_ms(10); //去抖动
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
    return 0; // 无按键按下
}
extern EventGroupHandle_t Key_EventGroupHandle;
void EXTI9_5_IRQHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken;
    delay_xms(10); //消抖
    if (SW3 == 0)
    {
        xEventGroupSetBitsFromISR(Key_EventGroupHandle, SW3_EVENTGROUPS_BIT, &pxHigherPriorityTaskWoken);
    }
    if (SW4 == 0)
    {
        xEventGroupSetBitsFromISR(Key_EventGroupHandle, SW4_EVENTGROUPS_BIT, &pxHigherPriorityTaskWoken);
    }
    EXTI_ClearITPendingBit(EXTI_Line8 | EXTI_Line9); //清除LINE9和8上的中断标志位
}
