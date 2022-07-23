/*
 * @Description: timer.c 包含定时器和编码器初始化,PWM等函数实现以及相关中断
 * @Author: TOTHTOT
 * @Date: 2022-07-14 16:57:55
 * @LastEditTime: 2022-07-21 13:42:14
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\TIMER\timer.c
 */
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#include "timer.h"
#include "tb6612.h"
#include "usart.h"
#include "Beep.h"
/**
 * @name: Timer2_PWM_Init
 * @msg: 定时器2输出4路PWM 20KHz
 * PA15:CH1, PB3:CH2 PA2:CH3, PA3:CH4
 * @return {*}
 */
void Timer2_PWM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 初始化时钟和复用
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    /*  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);    // 完全重映射
     GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //禁用JTAG
      */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //禁止JTAG功能，把PB3，PB4作为普通IO口使用

    // 初始化IO口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // PWM-IO
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // GPIO_Mode_AF_PP
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11; // PWM-IO
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // GPIO_Mode_AF_PP
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始化定时器输出PWM 72M/20K=3600
    TIM_DeInit(TIM2);                                           //将外设 TIMx 寄存器重设为缺省值
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置了在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置了用来作为 TIM1 时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 设置了时钟不分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //选择了计数器模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);             //根据 TIM_TimeBaseInitStruct 中指定的参数初始化 TIMx 的时间基数单位

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //选择定时器模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态
    TIM_OCInitStructure.TIM_Pulse = 0;                            //设置了待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性

    TIM_OC1Init(TIM2, &TIM_OCInitStructure); //根据 TIM2_OCInitStruct 中指定的参数初始化 TIM2通道 1
    TIM_OC2Init(TIM2, &TIM_OCInitStructure); //根据 TIM2_OCInitStruct 中指定的参数初始化 TIM2通道 2
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); //根据 TIM2_OCInitStruct 中指定的参数初始化 TIM2通道 3
    TIM_OC4Init(TIM2, &TIM_OCInitStructure); //根据 TIM2_OCInitStruct 中指定的参数初始化 TIM2通道 4

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); //使能或者失能 TIMx 在 CCR1 上的预装载寄存器
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); //使能或者失能 TIMx 在 CCR1 上的预装载寄存器
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); //使能或者失能 TIMx 在 CCR1 上的预装载寄存器
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); //使能或者失能 TIMx 在 CCR1 上的预装载寄存器

    TIM_CtrlPWMOutputs(TIM2, ENABLE);   // MOE 主输出使能
    TIM_ARRPreloadConfig(TIM2, ENABLE); //使能或者失能 TIMx 在 ARR 上的预装载寄存器

    TIM_Cmd(TIM2, ENABLE); //使能或者失能 TIMx 外设
}

/**
 * @name: Timer3_Encoder_Init
 * @msg: 定时器3编码器模式 电机1使用
 * @return {*}
 */
void Timer3_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //使能定时器2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //根据设定参数初始GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 预分频器
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising,
                               TIM_ICPolarity_Rising); //使用编码器模式3，模式3就我们在这里所说的4倍频，详细信息查看stm32f1技术手册

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update); //清除TIM的更新标志位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    // Reset counter

    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @name: Timer4_Encoder_Init
 * @msg: 定时器4编码器模式 电机2使用
 * @return {*}
 */
void Timer4_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //使能定时器4的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //根据设定参数初始化GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 预分频器
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //使用编码器模式3

    TIM_ICStructInit(&TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update); //清除TIM的更新标志位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    // Reset counter
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

/**
 * @name: Timer5_Encoder_Init
 * @msg: 定时器5编码器模式 电机3使用
 * @return {*}
 */
void Timer5_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //使能定时器5的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0; //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //根据设定参数初始化GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 预分频器
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //使用编码器模式3

    TIM_ICStructInit(&TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update); //清除TIM的更新标志位
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    // Reset counter
    TIM_SetCounter(TIM5, 0);
    TIM_Cmd(TIM5, ENABLE);
}

/**
 * @name: Timer8_Encoder_Init
 * @msg: 定时器8编码器模式 电机4使用
 * @return {*}
 */
void Timer8_Encoder_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  //使能定时器5的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);                 //根据设定参数初始化GPIOB

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 预分频器
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //使用编码器模式3

    TIM_ICStructInit(&TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM8, TIM_FLAG_Update); //清除TIM的更新标志位
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
    // Reset counter
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM8, ENABLE);
}

/**
 * @name: Read_Encode_Num
 * @msg: 读取脉冲数, 方向还没改
 * @param {u8} TIMx定时器
 * @return {*}
 */
int Read_Encode_Num(u8 TIMx)
{
    switch (TIMx)
    {
    case 3:
        Car_1.motro1_state.encode_num = -((short)TIM3->CNT);                  //获取脉冲数
        Car_1.motro1_state.total_encode_num += Car_1.motro1_state.encode_num; //总脉冲数相加
        TIM3->CNT = 0;                                                        //清零
        break;
    case 4:
        Car_1.motro2_state.encode_num = -((short)TIM4->CNT);                  //获取脉冲数
        Car_1.motro2_state.total_encode_num += Car_1.motro2_state.encode_num; //总脉冲数相加
        TIM4->CNT = 0;                                                        //清零
        break;
    case 5:
        Car_1.motro3_state.encode_num = -((short)TIM5->CNT);                  //获取脉冲数
        Car_1.motro3_state.total_encode_num += Car_1.motro3_state.encode_num; //总脉冲数相加
        TIM5->CNT = 0;                                                        //清零
        break;
    case 8:
        Car_1.motro4_state.encode_num = -((short)TIM8->CNT);                  //获取脉冲数
        Car_1.motro4_state.total_encode_num += Car_1.motro4_state.encode_num; //总脉冲数相加
        TIM8->CNT = 0;                                                        //清零
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @name: TIM1_Init
 * @msg: 初始化定时器1,用于BEEP,因为没有其他的PWM输出口了
 *       所以只能使用中断方式改变电平,一次中断触发时间为10us,最大频率为100KHz
 * @param {u16} arr:装载值
 * @param {u16} psc分频值
 * @return {*}
 */
void Timer1_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVICinitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置了在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置了用来作为 TIM1 时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 设置了时钟不分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //选择了计数器模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据 TIM_TimeBaseInitStruct 中指定的参数初始化 TIMx 的时间基数单位

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);      //清中断标志位
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); //|TIM_IT_Trigger
    NVICinitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVICinitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVICinitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVICinitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVICinitStructure);
    // TIM_Cmd(TIM1, ENABLE); //使能TIMx外设
}

extern SemaphoreHandle_t BEEP_Bin_SemaphoreHandle;
void TIM1_UP_IRQHandler(void)
{
    static u16 i;
    BaseType_t pxHigherPriorityTaskWoken;
    BaseType_t xTaskWokenByReceive = pdFALSE;
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        if ((Beep.beep_en == beep_enable || Beep.beep_en == beep_busy) && (int)Beep.pinlv >= 0)
        {
            Beep.pinlv = Beep.pinlv - 10.0;
            i++;
            // printf("i:%d, pv:%d\r\n", i, (int)Beep.pinlv);
            Beep.beep_en = beep_busy;
            if (Beep.high_time < (int)Beep.pinlv)
            {
                BEEP_HIGH;
            }
            else
            {
                BEEP_LOW;
            }
        }
        else if (Beep.jiepai > 0)
        {
            Beep.jiepai--;
            Beep.pinlv = Beep.pinlv_m;
        }
        else
        {
            BEEP_OFF;
            i = 0;
            // printf("Beep\r\n");
            if (Beep.beep_play_music == beep_enable)
                xSemaphoreGiveFromISR(BEEP_Bin_SemaphoreHandle, &pxHigherPriorityTaskWoken);
            Beep.beep_en = beep_disable;
        }
    }
    portYIELD_FROM_ISR(xTaskWokenByReceive); //如果需要的话进行一次任务切换
}
