/*
 * @Author: TOTHTOT
 * @Date: 2022-03-31 17:22:34
 * @LastEditTime: 2022-07-22 21:20:13
 * @LastEditors: TOTHTOT
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4轮小车\HARDWARE\USART\usart2.h
 */
#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"
#include "sys.h"

#define false 0
#define true 1

//定义数组长度
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2

#define USART2_MAX_RECV_LEN 200 //最大接收缓存字节数
#define USART2_MAX_SEND_LEN 400 //最大发送缓存字节数
#define USART2_RX_EN 1          // 0,不接收;1,接收.

typedef struct
{
    u8 USARTx_RX_BUF1[USART2_MAX_RECV_LEN]; //接收缓冲,最大USART2_MAX_RECV_LEN字节
    u8 USARTx_RX_BUF2[USART2_MAX_RECV_LEN]; //接收缓冲,最大USART2_MAX_RECV_LEN字节
    u8 USARTx_TX_BUF[USART2_MAX_SEND_LEN]; //发送缓冲,最大USART2_MAX_SEND_LEN字节
    u8 witchbuf;  
    u16 USARTx_RX_STA;                     //接收数据状态
    u8 USARTx_TX_FLAG;
}FIFO_DMA_H;
extern FIFO_DMA_H Usart2_Handle;
void USART2_Init(u32 My_BaudRate);
void u2_printf(char *fmt, ...);
void Usart2_DMA_Init(void);
void DMA_USART2_Tx_Data(u8 *buffer, u32 size);

#endif
