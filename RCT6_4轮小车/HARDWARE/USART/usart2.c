#include "delay.h"
#include "usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "queue.h"

/* 和ATGM336H通信 */
/* //串口接收缓存区
u8 USART2_RX_BUF1[USART2_MAX_RECV_LEN]; //接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u8 USART2_RX_BUF2[USART2_MAX_RECV_LEN]; //接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];  //发送缓冲,最大USART2_MAX_SEND_LEN字节
//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART2_RX_STA = 0;
 */
FIFO_DMA_H Usart2_Handle;
extern xSemaphoreHandle GPS_Bin_Semaphore;
void USART2_IRQHandler(void)
{
    u8 *p;
    u8 USART2_RX_LEN = 0;                                  //接收数据长度
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //串口2空闲中断
    {
        USART_ReceiveData(USART2);                                  //清除串口2空闲中断IDLE标志位
        USART_ClearFlag(USART2, USART_FLAG_TC);                     //清除USART2标志位
        DMA_Cmd(DMA1_Channel6, DISABLE);                            //关闭USART2 TX DMA1 所指示的通道
        USART2_RX_LEN = USART2_MAX_RECV_LEN - DMA1_Channel6->CNDTR; //获得接收到的字节数
        if (Usart2_Handle.witchbuf)                                 //之前用的u2rxbuf，切换为u1rxbuf
        {
            p = Usart2_Handle.USARTx_RX_BUF2;                        //先保存前一次数据地址再切换缓冲区
            DMA1_Channel6->CMAR = (u32)Usart2_Handle.USARTx_RX_BUF1; //切换为u1rxbuf缓冲区地址
            Usart2_Handle.witchbuf = 0;                              //下一次切换为u2rxbuf
        }
        else //之前用的u1rxbuf，切换为u2rxbuf
        {
            p = Usart2_Handle.USARTx_RX_BUF1;                        //先保存前一次数据地址再切换缓冲区
            DMA1_Channel6->CMAR = (u32)Usart2_Handle.USARTx_RX_BUF2; //切换为u2rxbuf缓冲区地址
            Usart2_Handle.witchbuf = 1;                              //下一次切换为u1rxbuf
        }
        DMA1_Channel6->CNDTR = USART2_MAX_RECV_LEN; // DMA通道的DMA缓存的大小
        DMA_Cmd(DMA1_Channel6, ENABLE);             //使能USART2 TX DMA1 所指示的通道

        //******************↓↓↓↓↓这里作数据处理↓↓↓↓↓******************//

        DMA_USART2_Tx_Data(p, USART2_RX_LEN);

        //******************↑↑↑↑↑这里作数据处理↑↑↑↑↑******************//
    }
}

//初始化IO 串口2 DMA方式发送接收
// pclk1:PCLK1时钟频率(Mhz)
// bound:波特率
void USART2_Init(u32 bound)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //串口2时钟使能

    USART_DeInit(USART2);                     //复位串口2
                                              // USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);          //初始化PA2

    // USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);        //初始化PA3

    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口2

    USART_Cmd(USART2, ENABLE); //使能串口

    //使能接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //开启中断
                                                   //空闲中断开启设置
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); //开启检测串口空闲状态中断
    USART_ClearFlag(USART2, USART_FLAG_TC);

    //设置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11; //抢占优先级6
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                            //根据指定的参数初始化VIC寄存器
    // dma设置
    Usart2_DMA_Init();
}

void Usart2_DMA_Init(void)
{
    DMA_InitTypeDef DMA1_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // DMA_USART2_RX  USART2->RAM的数据传输
    DMA_DeInit(DMA1_Channel6);                                                 //将DMA的通道6寄存器重设为缺省值
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);            //启动传输前装入实际RAM地址
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_Handle.USARTx_RX_BUF1; //设置接收缓冲区首地址
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                        //数据传输方向，从外设读取到内存
    DMA1_InitStructure.DMA_BufferSize = USART2_MAX_RECV_LEN;                   // DMA通道的DMA缓存的大小
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //外设地址寄存器不变
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //内存地址寄存器递增
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //数据宽度为8位
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //数据宽度为8位
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                             //工作在正常模式
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                       // DMA通道 x拥有高优先级
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                              // DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel6, &DMA1_InitStructure);

    // DMA_USART2_TX  RAM->USART2的数据传输
    DMA_DeInit(DMA1_Channel7);                                                //将DMA的通道7寄存器重设为缺省值
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);           //启动传输前装入实际RAM地址
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_Handle.USARTx_TX_BUF; //设置发送缓冲区首地址
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                       //数据传输方向，从内存发送到外设
    DMA1_InitStructure.DMA_BufferSize = USART2_MAX_SEND_LEN;                  // DMA通道的DMA缓存的大小
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //外设地址寄存器不变
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //内存地址寄存器递增
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          //数据宽度为8位
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                            //工作在正常模式
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                      // DMA通道 x拥有高优先级
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                             // DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel7, &DMA1_InitStructure);

    // DMA1通道6 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;  // NVIC通道设置
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器

    // DMA1通道7 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;  // NVIC通道设置
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器

    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); //开USART2 Rx DMA中断
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE); //开USART2 Tx DMA中断
    DMA_Cmd(DMA1_Channel6, ENABLE);                 //使DMA通道6停止工作
                                                    // 笔记:没数据要发送时关闭发送DMA
    DMA_Cmd(DMA1_Channel7, DISABLE);                //使DMA通道7停止工作

    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //开启串口DMA发送
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char *format, ...)
{

    // //VA_LIST 是在C语言中解决变参问题的一组宏，所在头文件：#include <stdarg.h>，用于获取不确定个数的参数。
    va_list arg_ptr; //实例化可变长参数列表

    while (Usart2_Handle.USARTx_TX_FLAG)
        ; //等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）

    va_start(arg_ptr, format); //初始化可变参数列表，设置format为可变长列表的起始点（第一个元素）

    // USART2_MAX_TX_LEN+1可接受的最大字符数(非字节数，UNICODE一个字符两个字节), 防止产生数组越界
    vsnprintf((char *)Usart2_Handle.USARTx_TX_BUF, USART2_MAX_SEND_LEN + 1, format, arg_ptr); //从USART2_TX_BUF的首地址开始拼合，拼合format内容；USART2_MAX_TX_LEN+1限制长度，防止产生数组越界

    va_end(arg_ptr); //注意必须关闭

    DMA_USART2_Tx_Data(Usart2_Handle.USARTx_TX_BUF, strlen((const char *)Usart2_Handle.USARTx_TX_BUF)); //发送USART2_TX_BUF内容
}

/**
 * @name: DMA_USART2_Tx_Data
 * @msg: DMA 发送应用源码 用sprintf拼接buffer
 * @return {*}
 */
void DMA_USART2_Tx_Data(u8 *buffer, u32 size)
{
    while (Usart2_Handle.USARTx_TX_FLAG)
        {
            printf("11\r\n");
        }                                   //等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）
    Usart2_Handle.USARTx_TX_FLAG = 1;       // USART2发送标志（启动发送）
    DMA1_Channel7->CMAR = (uint32_t)buffer; //设置要发送的数据地址
    DMA1_Channel7->CNDTR = size;            //设置要发送的字节数目
    DMA_Cmd(DMA1_Channel7, ENABLE);         //开始DMA发送
}

// DMA1通道7中断
void DMA1_Channel7_IRQHandler(void)
{
    printf("33\r\n");
    if (DMA_GetITStatus(DMA1_IT_TC7) != RESET) // DMA接收完成标志
    {
        printf("22\r\n");
        DMA_ClearITPendingBit(DMA1_IT_TC7);     //清除中断标志
        USART_ClearFlag(USART2, USART_FLAG_TC); //清除串口2的标志位
        DMA_Cmd(DMA1_Channel7, DISABLE);        //关闭USART2 TX DMA1 所指示的通道
        Usart2_Handle.USARTx_TX_FLAG = 0;       // USART2发送标志(关闭)
    }
}