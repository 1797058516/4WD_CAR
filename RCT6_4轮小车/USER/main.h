/*
 * @Description: main.h
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:40:37
 * @LastEditTime: 2022-07-22 16:52:36
 * @LastEditors: TOTHTOT
 * @FilePath: \USER\main.h
 */
#ifndef __MAIN_H
#define __MAIN_H

/*SYSTEM*/
#include "sys.h"
#include "delay.h"
#include "string.h"
#include "config.h"
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
/*HARDWARE*/
#include "led.h"
#include "timer.h"
#include "usart2.h"
#include "usart3.h"
#include "usart5.h"
#include "oled.h"
#include "key.h"
#include "tb6612.h"
#include "pid.h"
#include "adc.h"
#include "beep.h"
#include "music.h"
#include "tft.h"
<<<<<<< HEAD


=======
#include "bsp_usart_dma.h"
/* С�����Ժ���� */
#define TEST_ENCODE 0   //����������
#define USART2_TEST 0   //����2���Կ���
>>>>>>> main

//�������ȼ�
#define START_TASK_PRIO 1
//�����ջ��С
#define START_STK_SIZE 128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO 2
//�����ջ��С
#define LED0_STK_SIZE 50
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define OLED_TASK_PRIO 2
//�����ջ��С
#define OLED_STK_SIZE 150
//������
TaskHandle_t OLEDTask_Handler;
//������
void oled_task(void *pvParameters);

//�������ȼ�
#define PID_TASK_PRIO 20
//�����ջ��С
#define PID_STK_SIZE 260
//������
TaskHandle_t PIDTask_Handler;
//������
void pid_task(void *pvParameters);

//�������ȼ�
#define KEY_TASK_PRIO 3
//�����ջ��С
#define KEY_STK_SIZE 80
//������
TaskHandle_t KEYTask_Handler;
//������
void key_task(void *pvParameters);

//�������ȼ�
#define Play_Music_TASK_PRIO 3
//�����ջ��С
#define Play_Music_STK_SIZE 280
//������
TaskHandle_t Play_MusicTask_Handler;
//������
void play_music_task(void *pvParameters);

#endif
