#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 --------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/* 以下宏定义是YS-F4Pro开发板专门拉出来的12个引脚加上光耦作为输入端引脚 */
#define OUTPUT1_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT1_GPIO_PIN                 GPIO_PIN_12
#define OUTPUT1_GPIO                     GPIOF

#define OUTPUT2_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT2_GPIO_PIN                 GPIO_PIN_13
#define OUTPUT2_GPIO                     GPIOF

#define OUTPUT3_RCC_CLK_ENABLE          __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT3_GPIO_PIN                 GPIO_PIN_14
#define OUTPUT3_GPIO                     GPIOF

#define OUTPUT4_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT4_GPIO_PIN                 GPIO_PIN_15
#define OUTPUT4_GPIO                     GPIOF


#define OUTPUT1_GPIO_ON                       HAL_GPIO_WritePin(OUTPUT1_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define OUTPUT1_GPIO_OFF                      HAL_GPIO_WritePin(OUTPUT1_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_RESET)    // 输出低电平

#define OUTPUT2_GPIO_ON                       HAL_GPIO_WritePin(OUTPUT2_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define OUTPUT2_GPIO_OFF                      HAL_GPIO_WritePin(OUTPUT2_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_RESET)    // 输出低电平

#define OUTPUT3_GPIO_ON                       HAL_GPIO_WritePin(OUTPUT3_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define OUTPUT3_GPIO_OFF                      HAL_GPIO_WritePin(OUTPUT3_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_RESET)    // 输出低电平

#define OUTPUT4_GPIO_ON                       HAL_GPIO_WritePin(OUTPUT4_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define OUTPUT4_GPIO_OFF                      HAL_GPIO_WritePin(OUTPUT4_GPIO,OUTPUT1_GPIO_PIN,GPIO_PIN_RESET)    // 输出低电平

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void OUTPUT_GPIO_Init(void);

#endif  // __BSP_GPIO_H__

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
