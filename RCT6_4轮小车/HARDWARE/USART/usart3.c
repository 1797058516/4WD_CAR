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

//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
u8 cmd_flag;
vu16 USART3_RX_STA = 0;
u8 ReceiveData_Com = 0; //判断是否接收完字符串
u8 str_len, s1;


void USART3_IRQHandler(void)
{
	u8 res;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{
		res = USART_ReceiveData(USART3);
		USART_SendData(USART3, res);
		// USART3_RX_BUF[str_len] = res;
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

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); // GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_AFIO, ENABLE); // GPIOC时钟
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);						//串口3时钟使能
	//部分重映射
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE); 
	
	USART_DeInit(USART3); //复位串口
	// USART3_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//初始化PB10

	// USART3_RX	  PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);				  //初始化PB11

	USART_InitStructure.USART_BaudRate = bound;										//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口	3

	USART_Cmd(USART3, ENABLE); //使能串口

	//使能接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启中断

	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7; //抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  //根据指定的参数初始化VIC寄存器

	USART3_RX_STA = 0; //清零
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char *fmt, ...) //...表示可变参数（多个可变参数组成一个列表，后面有专门的指针指向他），不限定个数和类型
{
	u16 i, j;
	va_list ap;								  //初始化指向可变参数列表的指针
	va_start(ap, fmt);						  //将第一个可变参数的地址付给ap，即ap指向可变参数列表的开始
	vsprintf((char *)USART3_TX_BUF, fmt, ap); //将参数fmt、ap指向的可变参数一起转换成格式化字符串，放(char*)USART3_TX_BUF数组中，其作用同sprintf（），只是参数类型不同
	va_end(ap);
	i = strlen((const char *)USART3_TX_BUF); //此次发送数据的长度
	for (j = 0; j < i; j++)					 //循环发送数据
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			;									  //循环发送,直到发送完毕
		USART_SendData(USART3, USART3_TX_BUF[j]); //把格式化字符串从开发板串口送出去
	}
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


