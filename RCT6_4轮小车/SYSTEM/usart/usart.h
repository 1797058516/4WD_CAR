#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
typedef struct
{
    u8* USARTx_RX_BUF1; //���ջ���,���USART2_MAX_RECV_LEN�ֽ�
    u8* USARTx_RX_BUF2; //���ջ���,���USART2_MAX_RECV_LEN�ֽ�
    u8* USARTx_TX_BUF; //���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
		uint32_t rx_buf_size;  
		uint32_t tx_buf_size; 
    u8 witchbuf;  
    u16 USARTx_RX_STA;                     //��������״̬
    u8 USARTx_TX_FLAG;
}FIFO_DMA_H;	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);

void USART1_TX_DMA_Config(void);
void DMA_USART1_Tx_Data(u8 *buffer, u32 size);
void USART1_printf(char *format, ...);
void FIFO_DMA_INIT(FIFO_DMA_H* USARTx);
void u1_printf(char* format,...);

extern FIFO_DMA_H USARTD1;


#endif


