/*
 * @Description: 
 * @Author: TOTHTOT
 * @Date: 2022-07-23 14:27:07
 * @LastEditTime: 2022-07-23 15:13:23
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\MPU6050\mpuiic.h
 */
#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "sys.h"

//IO��������  ---PB13
#define MPU_SDA_IN()  {GPIOB->CRH &= 0XFF0FFFFF;GPIOB->CRH |= 8<<20;}   //����/���� ����ģʽ
#define MPU_SDA_OUT() {GPIOB->CRH &= 0XFF0FFFFF;GPIOB->CRH |= 3<<20;}		//�������  ���ģʽ

//IO��������	 
#define MPU_IIC_SCL    PBout(12) 		//SCL
#define MPU_IIC_SDA    PBout(13) 		//SDA	 
#define MPU_READ_SDA   PBin(13) 		//����SDA 

//IIC���в�������
void MPU_IIC_Delay(void);								//IIC��ʱ2ms����
void MPU_IIC_Init(void);                //��ʼ��IIC��IO��				 
void MPU_IIC_Start(void);								//����IIC��ʼ�ź�
void MPU_IIC_Stop(void);	  						//����IICֹͣ�ź�
void MPU_IIC_Send_Byte(u8 txd);					//IIC����һ���ֽ�
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 MPU_IIC_Wait_Ack(void); 							//IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(void);									//IIC����ACK�ź�
void MPU_IIC_NAck(void);								//IIC������ACK�ź�


void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif
