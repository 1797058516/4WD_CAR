/*
 * @Description: usart5.h
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:34:33
 * @LastEditTime: 2022-07-18 20:40:56
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\USART\usart5.h
 */
#ifndef __USART5_H
#define __USART5_H	 
#include "sys.h"  
 
#define USART5_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART5_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART5_RX_EN 			1					//0,不接收;1,接收.

extern u8  USART5_RX_BUF[USART5_MAX_RECV_LEN]; 		//接收缓冲,最大USART5_MAX_RECV_LEN字节
extern u8  USART5_TX_BUF[USART5_MAX_SEND_LEN]; 		//发送缓冲,最大USART5_MAX_SEND_LEN字节
extern vu16 USART5_RX_STA;   						//接收数据状态
extern u8 check_cmd_result, cmd_flag,str_len, s1;


void usart5_init(u32 bound);				//串口5初始化 
void u5_printf(char* fmt,...);
void USART5_RX_Data(void);
unsigned char u5_get_byte(unsigned char* GetData);

#endif
