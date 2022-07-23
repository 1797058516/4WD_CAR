/*
 * @Author: TOTHTOT
 * @Date: 2022-03-31 17:22:34
 * @LastEditTime: 2022-07-23 09:52:47
 * @LastEditors: TOTHTOT
 * @Description: ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\USART\usart3.h
 */
#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
 
#define USART3_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		600					//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.
#define OPENMV_WAITTIME         10                 //OPENMV�ȴ�ʱ��,��ʱ˵���ڴ�λ��û��ʶ����Ч����,��Сֵ���̵ȴ�ʶ��ʱ�� 10*100ms=1s

typedef struct
{
    u8 USARTx_RX_BUF1[USART3_MAX_RECV_LEN]; //���ջ���,���USART2_MAX_RECV_LEN�ֽ�
    u8 USARTx_RX_BUF2[USART3_MAX_RECV_LEN]; //���ջ���,���USART2_MAX_RECV_LEN�ֽ�
    u8 USARTx_TX_BUF[USART3_MAX_SEND_LEN]; //���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
    u8 witchbuf;  
    u16 USARTx_RX_STA;                     //��������״̬
    u8 USARTx_TX_FLAG;
}FIFO2_DMA_H;

extern FIFO2_DMA_H Usart3_Handle;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern vu16 USART3_RX_STA;   						//��������״̬
extern u8 check_cmd_result, cmd_flag,str_len, s1;


void usart3_init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt,...);
void USART3_RX_Data(void);
unsigned char u3_get_byte(unsigned char* GetData);
void Usart3_DMA_Init(void);
void DMA_USART3_Tx_Data(u8 *buffer, u32 size);

#endif
 
 
 
 
 
 
 
 
 
 
 
 

