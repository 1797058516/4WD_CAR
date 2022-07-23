/* SYSTEM */
#include "delay.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "string.h"
/* HARDWARE */
#include "usart3.h"
#include "usart.h"
#include "led.h"
#include "oled.h"

FIFO2_DMA_H Usart3_Handle;

void USART3_IRQHandler(void)
{
    u8 *p;
    u8 USART3_RX_LEN = 0;                                  //接收数据长度
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //串口3空闲中断
    {
        USART_ReceiveData(USART3);                                  //清除串口2空闲中断IDLE标志位
        USART_ClearFlag(USART3, USART_FLAG_TC);                     //清除USART2标志位
        DMA_Cmd(DMA1_Channel3, DISABLE);                            //关闭USART2 TX DMA1 所指示的通道
        USART3_RX_LEN = USART3_MAX_RECV_LEN - DMA1_Channel3->CNDTR; //获得接收到的字节数
        if (Usart3_Handle.witchbuf)                                 //之前用的u2rxbuf，切换为u1rxbuf
        {
            p = Usart3_Handle.USARTx_RX_BUF2;                        //先保存前一次数据地址再切换缓冲区
            DMA1_Channel3->CMAR = (u32)Usart3_Handle.USARTx_RX_BUF1; //切换为u1rxbuf缓冲区地址
            Usart3_Handle.witchbuf = 0;                              //下一次切换为u2rxbuf
        }
        else //之前用的u1rxbuf，切换为u2rxbuf
        {
            p = Usart3_Handle.USARTx_RX_BUF1;                        //先保存前一次数据地址再切换缓冲区
            DMA1_Channel3->CMAR = (u32)Usart3_Handle.USARTx_RX_BUF2; //切换为u2rxbuf缓冲区地址
            Usart3_Handle.witchbuf = 1;                              //下一次切换为u1rxbuf
        }
        DMA1_Channel3->CNDTR = USART3_MAX_RECV_LEN; // DMA通道的DMA缓存的大小
        DMA_Cmd(DMA1_Channel3, ENABLE);             //使能USART2 TX DMA1 所指示的通道

        //******************↓↓↓↓↓这里作数据处理↓↓↓↓↓******************//

        DMA_USART3_Tx_Data(p, USART3_RX_LEN);

        //******************↑↑↑↑↑这里作数据处理↑↑↑↑↑******************//
    }
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}

//初始化IO 串口3
// pclk1:PCLK1时钟频率(Mhz)
// bound:波特率
void usart3_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); // GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); // GPIOC时钟

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //串口3时钟使能
    //部分重映射
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);

    USART_DeInit(USART3); //复位串口
    // USART3_TX   PC10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);          //初始化PB10

    // USART3_RX	  PC11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);                //初始化PB11

    USART_InitStructure.USART_BaudRate = bound;                                     //波特率一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口	3

    USART_Cmd(USART3, ENABLE); //使能串口

    //使能接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启中断
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); //开启检测串口空闲状态中断
    USART_ClearFlag(USART3, USART_FLAG_TC);
    //设置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14; //抢占优先级7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化VIC寄存器

    Usart3_DMA_Init();
}

void Usart3_DMA_Init(void)
{
    DMA_InitTypeDef DMA1_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // DMA_USART3_RX  USART3->RAM的数据传输
    DMA_DeInit(DMA1_Channel3);                                                 //将DMA的通道6寄存器重设为缺省值
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);            //启动传输前装入实际RAM地址
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart3_Handle.USARTx_RX_BUF1; //设置接收缓冲区首地址
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                        //数据传输方向，从外设读取到内存
    DMA1_InitStructure.DMA_BufferSize = USART3_MAX_RECV_LEN;                   // DMA通道的DMA缓存的大小
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //外设地址寄存器不变
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //内存地址寄存器递增
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //数据宽度为8位
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //数据宽度为8位
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                             //工作在正常模式
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                       // DMA通道 x拥有高优先级
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                              // DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel3, &DMA1_InitStructure);

    // DMA_USART3_TX  RAM->USART3的数据传输
    DMA_DeInit(DMA1_Channel2);                                                //将DMA的通道7寄存器重设为缺省值
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);           //启动传输前装入实际RAM地址
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart3_Handle.USARTx_TX_BUF; //设置发送缓冲区首地址
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                       //数据传输方向，从内存发送到外设
    DMA1_InitStructure.DMA_BufferSize = USART3_MAX_SEND_LEN;                  // DMA通道的DMA缓存的大小
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //外设地址寄存器不变
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //内存地址寄存器递增
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          //数据宽度为8位
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                            //工作在正常模式
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                      // DMA通道 x拥有高优先级
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                             // DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel2, &DMA1_InitStructure);

    // DMA1通道3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;  // NVIC通道设置
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器

    // DMA1通道2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;  // NVIC通道设置
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器

    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE); //开USART3 Rx DMA中断
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE); //开USART3 Tx DMA中断
    DMA_Cmd(DMA1_Channel3, ENABLE);                 //使DMA通道3停止工作
                                                    // 笔记:没数据要发送时关闭发送DMA
    DMA_Cmd(DMA1_Channel2, DISABLE);                //使DMA通道2停止工作

    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); //开启串口DMA发送
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char *format, ...) //...表示可变参数（多个可变参数组成一个列表，后面有专门的指针指向他），不限定个数和类型
{
    va_list arg_ptr; //实例化可变长参数列表

    while (Usart3_Handle.USARTx_TX_FLAG)
        ; //等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）

    va_start(arg_ptr, format); //初始化可变参数列表，设置format为可变长列表的起始点（第一个元素）

    // USART2_MAX_TX_LEN+1可接受的最大字符数(非字节数，UNICODE一个字符两个字节), 防止产生数组越界
    vsnprintf((char *)Usart3_Handle.USARTx_TX_BUF, USART3_MAX_SEND_LEN + 1, format, arg_ptr); //从USART2_TX_BUF的首地址开始拼合，拼合format内容；USART2_MAX_TX_LEN+1限制长度，防止产生数组越界

    va_end(arg_ptr); //注意必须关闭

    DMA_USART3_Tx_Data(Usart3_Handle.USARTx_TX_BUF, strlen((const char *)Usart3_Handle.USARTx_TX_BUF)); //发送USART2_TX_BUF内容
}

/*发送一个字节数据*/
void u3_send_byte(unsigned char SendData)
{
    USART_SendData(USART3, SendData);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
        ;
}

/*接收一个字节数据*/
unsigned char u3_get_byte(unsigned char *GetData)
{
    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
    {
        return 0; //没有收到数据
    }
    *GetData = USART_ReceiveData(USART3);
    return 1; //收到数据
}

// DMA 发送应用源码
void DMA_USART3_Tx_Data(u8 *buffer, u32 size)
{
    while (Usart3_Handle.USARTx_TX_FLAG)
        {
            printf("11\r\n");
        };                                   //等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）
    Usart3_Handle.USARTx_TX_FLAG = 1;       // USART2发送标志（启动发送）
    DMA1_Channel2->CMAR = (uint32_t)buffer; //设置要发送的数据地址
    DMA1_Channel2->CNDTR = size;            //设置要发送的字节数目
    DMA_Cmd(DMA1_Channel2, ENABLE);         //开始DMA发送
}

// DMA1通道2中断
void DMA1_Channel2_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC2) != RESET) // DMA接收完成标志
    {
        DMA_ClearITPendingBit(DMA1_IT_TC2);     //清除中断标志
        USART_ClearFlag(USART3, USART_FLAG_TC); //清除串口3的标志位
        DMA_Cmd(DMA1_Channel2, DISABLE);        //关闭USART3 TX DMA1 所指示的通道
        Usart3_Handle.USARTx_TX_FLAG = 0;       // USART3发送标志(关闭)
    }
}
