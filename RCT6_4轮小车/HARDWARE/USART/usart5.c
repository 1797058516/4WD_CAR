/*
 * @Description: usart5.c
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:33:56
 * @LastEditTime: 2022-07-18 20:49:06
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\USART\usart5.c
 */
#include "usart5.h"
u8  USART5_RX_BUF[USART5_MAX_RECV_LEN];
void usart5_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);                        //使能UART5，GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); // GPIOC/D时钟
    USART_DeInit(UART5);                                                         //复位串口5
    // UART5_TX   PC12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // PC12
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);          //初始化PC12
    // UART5_RX	  PD2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);        //初始化PB11

    // Uart5 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器

    // UART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(UART5, &USART_InitStructure);                                        //初始化串口

    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //开启中断
    USART_Cmd(UART5, ENABLE);                     //使能串口

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

void UART5_IRQHandler(void)
{
    u8 res;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//接收中断
    {      
            res=UART5->DR;//读取接收到的数据; 
            USART_SendData(USART3, res);         		     
    }
    USART_ClearITPendingBit(UART5, USART_IT_RXNE);
}
