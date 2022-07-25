#include "sys.h"
#include "usart.h"

#include "string.h"
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOS使用
#endif
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32开发板
//串口1初始化
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2009-2019
// All rights reserved
//********************************************************************************
// V1.3修改说明
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
// V1.4修改说明
// 1,修改串口初始化IO的bug
// 2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
// 3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
// 4,修改了EN_USART1_RX的使能方式
// V1.5修改说明
// 1,增加了对UCOSII的支持
//////////////////////////////////////////////////////////////////////////////////

#define USART1_MAX_TX_LEN 100
#define USART1_MAX_RX_LEN 100

// USART2_MAX_TX_LEN和USART2_MAX_RX_LEN在头文件进行了宏定义，分别指USART2最大发送长度和最大接收长度
u8 USART1_TX_BUF[USART1_MAX_TX_LEN]; //发送缓冲,最大USART2_MAX_TX_LEN字节
u8 u1rxbuf[USART1_MAX_RX_LEN];       //发送数据缓冲区1
u8 u2rxbuf[USART1_MAX_RX_LEN];       //发送数据缓冲区2
u8 witchbuf = 0;                     //标记当前使用的是哪个缓冲区,0：使用u1rxbuf；1：使用u2rxbuf
u8 USART1_TX_FLAG = 0;               // USART2发送标志，启动发送时置1
u8 USART1_RX_FLAG = 0;               // USART2接收标志，启动接收时置1

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    USART1->DR = (u8)ch;
    // DMA_USART1_Tx_Data( &ch, 1);

    return ch;
}
#endif

#if EN_USART1_RX //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
// bit15，	接收完成标志
// bit14，	接收到0x0d
// bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0; //接收状态标记

void uart_init(u32 bound)
{
    // GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //使能USART1，GPIOA时钟

    // USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);          //初始化GPIOA.9

    // USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                //初始化GPIOA.10

    // Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器

    // USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;                                     //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式

    USART_Init(USART1, &USART_InitStructure);      //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启串口接受中断
    USART_Cmd(USART1, ENABLE);                     //使能串口1
}

void USART1_IRQHandler(void) //串口1中断服务程序
{
    u8 Res;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART1); //读取接收到的数据

        if ((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART_RX_STA & 0x4000) //接收到了0x0d
            {
                if (Res != 0x0a)
                    USART_RX_STA = 0; //接收错误,重新开始
                else
                    USART_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (Res == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
    }
}
#endif

// void USART1_TX_DMA_Config(uint8_t *mem_addr, uint32_t mem_size)
// DMA初始化的时候可以初始化地址和大小，也可以不初始化地址和大小

void USART1_TX_DMA_Config()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 设置DMA源地址：串口数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);

    // 内存地址(要传输的变量的指针)
    // DMA_InitStructure.DMA_MemoryBaseAddr = (u32)mem_addr;

    // 方向：从内存到外设
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

    // 传输大小
    //		DMA_InitStructure.DMA_BufferSize = mem_size;

    // 外设地址不增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // 内存地址自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 外设数据单位
    DMA_InitStructure.DMA_PeripheralDataSize =
        DMA_PeripheralDataSize_Byte;
    // 内存数据单位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // DMA模式，一次或者循环模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    // 优先级：中
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    // 禁止内存到内存的传输
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    // 配置DMA通道
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    /* DMA中断 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  /* UART1 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  /* UART1 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); //开USART1 Rx DMA中断
                                                    //	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
                                                    // 使能DMA
    DMA_Cmd(DMA1_Channel4, ENABLE);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* 使能DMA收发 */
}

// DMA 发送应用源码
void DMA_USART1_Tx_Data(u8 *buffer, u32 size)
{
    while (USART1_TX_FLAG)
        ;                                   //等待上一次发送完成（USART1_TX_FLAG为1即还在发送数据）
    USART1_TX_FLAG = 1;                     // USART1发送标志（启动发送）
    DMA1_Channel4->CMAR = (uint32_t)buffer; //设置要发送的数据地址
    DMA1_Channel4->CNDTR = size;            //设置要发送的字节数目
    DMA_Cmd(DMA1_Channel4, ENABLE);         //开始DMA发送
}
// DMA1通道7中断
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) // DMA接收完成标志
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);     //清除中断标志
        USART_ClearFlag(USART1, USART_FLAG_TC); //清除串口1的标志位
        DMA_Cmd(DMA1_Channel4, DISABLE);        //关闭USART1 TX DMA1 所指示的通道
        USART1_TX_FLAG = 0;                     // USART1发送标志(关闭)
    }
}

// void USART1_printf(char *format, ...)
//{
//	//VA_LIST 是在C语言中解决变参问题的一组宏，所在头文件：#include <stdarg.h>，用于获取不确定个数的参数。
//	va_list arg_ptr;														//实例化可变长参数列表
//
//	while(USART1_TX_FLAG);													//等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）
//
//	va_start(arg_ptr, format); 												//初始化可变参数列表，设置format为可变长列表的起始点（第一个元素）
//
//	// USART2_MAX_TX_LEN+1可接受的最大字符数(非字节数，UNICODE一个字符两个字节), 防止产生数组越界
//	vsnprintf((char*)USARTD1.USARTx_RX_BUF1, USARTD1.rx_buf_size+1, format, arg_ptr);	//从USART2_TX_BUF的首地址开始拼合，拼合format内容；USART2_MAX_TX_LEN+1限制长度，防止产生数组越界
//
//	va_end(arg_ptr);														//注意必须关闭
//
//   DMA_USART1_Tx_Data(USARTD1.USARTx_TX_BUF,USARTD1.tx_buf_size);
//	//DMA_USART1_Tx_Data(USARTD1.USARTx_TX_BUF,strlen((const char*)USART1_TX_BUF));
// }

void u1_printf(char *format, ...)
{
    // VA_LIST 是在C语言中解决变参问题的一组宏，所在头文件：#include <stdarg.h>，用于获取不确定个数的参数。
    va_list arg_ptr; //实例化可变长参数列表

    while (USART1_TX_FLAG)
        ; //等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）

    va_start(arg_ptr, format); //初始化可变参数列表，设置format为可变长列表的起始点（第一个元素）

    // USART2_MAX_TX_LEN+1可接受的最大字符数(非字节数，UNICODE一个字符两个字节), 防止产生数组越界
    vsnprintf((char *)USART1_TX_BUF, USART1_MAX_TX_LEN + 1, format, arg_ptr); //从USART2_TX_BUF的首地址开始拼合，拼合format内容；USART2_MAX_TX_LEN+1限制长度，防止产生数组越界

    va_end(arg_ptr); //注意必须关闭

    DMA_USART1_Tx_Data(USART1_TX_BUF, strlen((const char *)USART1_TX_BUF));
}

