/*
 * @Description: main.h
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:40:37
 * @LastEditTime: 2022-07-25 10:21:19
 * @LastEditors: TOTHTOT
 * @FilePath: \USER\main.h
 */
#ifndef __MAIN_H
#define __MAIN_H	 

/*SYSTEM*/
#include "sys.h"
#include "delay.h"
#include "string.h"
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
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "bsp_usart_dma.h"
#include "crc16.h"

/* 小车测试宏控制 */
#define TEST_ENCODE 1   //编码器测试
#define USART2_TEST 0   //串口2测试控制

//任务优先级
#define START_TASK_PRIO 1
//任务堆栈大小
#define START_STK_SIZE 188
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO 2
//任务堆栈大小
#define LED0_STK_SIZE 150
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define OLED_TASK_PRIO 2
//任务堆栈大小
#define OLED_STK_SIZE 150
//任务句柄
TaskHandle_t OLEDTask_Handler;
//任务函数
void oled_task(void *pvParameters);

//任务优先级
#define PID_TASK_PRIO 20
//任务堆栈大小
#define PID_STK_SIZE 360
//任务句柄
TaskHandle_t PIDTask_Handler;
//任务函数
void pid_task(void *pvParameters);

//任务优先级
#define KEY_TASK_PRIO 3
//任务堆栈大小
#define KEY_STK_SIZE 80
//任务句柄
TaskHandle_t KEYTask_Handler;
//任务函数
void key_task(void *pvParameters);

//任务优先级
#define Play_Music_TASK_PRIO 3
//任务堆栈大小
#define Play_Music_STK_SIZE 280
//任务句柄
TaskHandle_t Play_MusicTask_Handler;
//任务函数
void play_music_task(void *pvParameters);

//任务优先级
#define MPU6050_TASK_PRIO 4
//任务堆栈大小
#define MPU6050_STK_SIZE 180
//任务句柄
TaskHandle_t MPU6050Task_Handler;
//任务函数
void MPU6050_task(void *pvParameters);


#endif

