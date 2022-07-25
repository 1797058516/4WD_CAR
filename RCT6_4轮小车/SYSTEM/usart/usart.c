#include "sys.h"
#include "usart.h"

#include "string.h"
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////////////
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOSʹ��
#endif
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32������
//����1��ʼ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
// All rights reserved
//********************************************************************************
// V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
// V1.4�޸�˵��
// 1,�޸Ĵ��ڳ�ʼ��IO��bug
// 2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
// 3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
// 4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
// V1.5�޸�˵��
// 1,�����˶�UCOSII��֧��
//////////////////////////////////////////////////////////////////////////////////

#define USART1_MAX_TX_LEN 100
#define USART1_MAX_RX_LEN 100

// USART2_MAX_TX_LEN��USART2_MAX_RX_LEN��ͷ�ļ������˺궨�壬�ֱ�ָUSART2����ͳ��Ⱥ������ճ���
u8 USART1_TX_BUF[USART1_MAX_TX_LEN]; //���ͻ���,���USART2_MAX_TX_LEN�ֽ�
u8 u1rxbuf[USART1_MAX_RX_LEN];       //�������ݻ�����1
u8 u2rxbuf[USART1_MAX_RX_LEN];       //�������ݻ�����2
u8 witchbuf = 0;                     //��ǵ�ǰʹ�õ����ĸ�������,0��ʹ��u1rxbuf��1��ʹ��u2rxbuf
u8 USART1_TX_FLAG = 0;               // USART2���ͱ�־����������ʱ��1
u8 USART1_RX_FLAG = 0;               // USART2���ձ�־����������ʱ��1

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0)
        ; //ѭ������,ֱ���������
    USART1->DR = (u8)ch;
    // DMA_USART1_Tx_Data( &ch, 1);

    return ch;
}
#endif

#if EN_USART1_RX //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; //����״̬���

void uart_init(u32 bound)
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //ʹ��USART1��GPIOAʱ��

    // USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);          //��ʼ��GPIOA.9

    // USART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                //��ʼ��GPIOA.10

    // Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��VIC�Ĵ���

    // USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;                                     //���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure);      //��ʼ������1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //�������ڽ����ж�
    USART_Cmd(USART1, ENABLE);                     //ʹ�ܴ���1
}

void USART1_IRQHandler(void) //����1�жϷ������
{
    u8 Res;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART1); //��ȡ���յ�������

        if ((USART_RX_STA & 0x8000) == 0) //����δ���
        {
            if (USART_RX_STA & 0x4000) //���յ���0x0d
            {
                if (Res != 0x0a)
                    USART_RX_STA = 0; //���մ���,���¿�ʼ
                else
                    USART_RX_STA |= 0x8000; //���������
            }
            else //��û�յ�0X0D
            {
                if (Res == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; //�������ݴ���,���¿�ʼ����
                }
            }
        }
    }
}
#endif

// void USART1_TX_DMA_Config(uint8_t *mem_addr, uint32_t mem_size)
// DMA��ʼ����ʱ����Գ�ʼ����ַ�ʹ�С��Ҳ���Բ���ʼ����ַ�ʹ�С

void USART1_TX_DMA_Config()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // ����DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);

    // �ڴ��ַ(Ҫ����ı�����ָ��)
    // DMA_InitStructure.DMA_MemoryBaseAddr = (u32)mem_addr;

    // ���򣺴��ڴ浽����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

    // �����С
    //		DMA_InitStructure.DMA_BufferSize = mem_size;

    // �����ַ����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // �ڴ��ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // �������ݵ�λ
    DMA_InitStructure.DMA_PeripheralDataSize =
        DMA_PeripheralDataSize_Byte;
    // �ڴ����ݵ�λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // DMAģʽ��һ�λ���ѭ��ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    // ���ȼ�����
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    // ��ֹ�ڴ浽�ڴ�Ĵ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    // ����DMAͨ��
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    /* DMA�ж� */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  /* UART1 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  /* UART1 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); //��USART1 Rx DMA�ж�
                                                    //	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
                                                    // ʹ��DMA
    DMA_Cmd(DMA1_Channel4, ENABLE);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* ʹ��DMA�շ� */
}

// DMA ����Ӧ��Դ��
void DMA_USART1_Tx_Data(u8 *buffer, u32 size)
{
    while (USART1_TX_FLAG)
        ;                                   //�ȴ���һ�η�����ɣ�USART1_TX_FLAGΪ1�����ڷ������ݣ�
    USART1_TX_FLAG = 1;                     // USART1���ͱ�־���������ͣ�
    DMA1_Channel4->CMAR = (uint32_t)buffer; //����Ҫ���͵����ݵ�ַ
    DMA1_Channel4->CNDTR = size;            //����Ҫ���͵��ֽ���Ŀ
    DMA_Cmd(DMA1_Channel4, ENABLE);         //��ʼDMA����
}
// DMA1ͨ��7�ж�
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) // DMA������ɱ�־
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);     //����жϱ�־
        USART_ClearFlag(USART1, USART_FLAG_TC); //�������1�ı�־λ
        DMA_Cmd(DMA1_Channel4, DISABLE);        //�ر�USART1 TX DMA1 ��ָʾ��ͨ��
        USART1_TX_FLAG = 0;                     // USART1���ͱ�־(�ر�)
    }
}

// void USART1_printf(char *format, ...)
//{
//	//VA_LIST ����C�����н����������һ��꣬����ͷ�ļ���#include <stdarg.h>�����ڻ�ȡ��ȷ�������Ĳ�����
//	va_list arg_ptr;														//ʵ�����ɱ䳤�����б�
//
//	while(USART1_TX_FLAG);													//�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�
//
//	va_start(arg_ptr, format); 												//��ʼ���ɱ�����б�����formatΪ�ɱ䳤�б����ʼ�㣨��һ��Ԫ�أ�
//
//	// USART2_MAX_TX_LEN+1�ɽ��ܵ�����ַ���(���ֽ�����UNICODEһ���ַ������ֽ�), ��ֹ��������Խ��
//	vsnprintf((char*)USARTD1.USARTx_RX_BUF1, USARTD1.rx_buf_size+1, format, arg_ptr);	//��USART2_TX_BUF���׵�ַ��ʼƴ�ϣ�ƴ��format���ݣ�USART2_MAX_TX_LEN+1���Ƴ��ȣ���ֹ��������Խ��
//
//	va_end(arg_ptr);														//ע�����ر�
//
//   DMA_USART1_Tx_Data(USARTD1.USARTx_TX_BUF,USARTD1.tx_buf_size);
//	//DMA_USART1_Tx_Data(USARTD1.USARTx_TX_BUF,strlen((const char*)USART1_TX_BUF));
// }

void u1_printf(char *format, ...)
{
    // VA_LIST ����C�����н����������һ��꣬����ͷ�ļ���#include <stdarg.h>�����ڻ�ȡ��ȷ�������Ĳ�����
    va_list arg_ptr; //ʵ�����ɱ䳤�����б�

    while (USART1_TX_FLAG)
        ; //�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�

    va_start(arg_ptr, format); //��ʼ���ɱ�����б�����formatΪ�ɱ䳤�б����ʼ�㣨��һ��Ԫ�أ�

    // USART2_MAX_TX_LEN+1�ɽ��ܵ�����ַ���(���ֽ�����UNICODEһ���ַ������ֽ�), ��ֹ��������Խ��
    vsnprintf((char *)USART1_TX_BUF, USART1_MAX_TX_LEN + 1, format, arg_ptr); //��USART2_TX_BUF���׵�ַ��ʼƴ�ϣ�ƴ��format���ݣ�USART2_MAX_TX_LEN+1���Ƴ��ȣ���ֹ��������Խ��

    va_end(arg_ptr); //ע�����ر�

    DMA_USART1_Tx_Data(USART1_TX_BUF, strlen((const char *)USART1_TX_BUF));
}

