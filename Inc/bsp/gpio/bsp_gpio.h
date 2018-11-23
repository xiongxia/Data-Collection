#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 --------------------------------------------------------------*/
typedef enum
{
  High   = 0,
  Low = 1,
}Input_State;//输入状态
/* 宏定义 --------------------------------------------------------------------*/
/* 以下宏定义是YS-F4Pro开发板专门拉出来的12个引脚加上光耦作为输入端引脚 */
#define INPUT1_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT1_GPIO_PIN                 GPIO_PIN_0
#define INPUT1_GPIO                     GPIOG
#define INPUT_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */

#define INPUT2_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT2_GPIO_PIN                 GPIO_PIN_1
#define INPUT2_GPIO                     GPIOG

#define INPUT3_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT3_GPIO_PIN                 GPIO_PIN_2
#define INPUT3_GPIO                     GPIOG


#define INPUT4_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT4_GPIO_PIN                 GPIO_PIN_3
#define INPUT4_GPIO                     GPIOG


#define INPUT5_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT5_GPIO_PIN                 GPIO_PIN_4
#define INPUT5_GPIO                     GPIOG


#define INPUT6_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT6_GPIO_PIN                 GPIO_PIN_5
#define INPUT6_GPIO                     GPIOG


#define INPUT7_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT7_GPIO_PIN                 GPIO_PIN_6
#define INPUT7_GPIO                     GPIOG


#define INPUT8_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT8_GPIO_PIN                 GPIO_PIN_7
#define INPUT8_GPIO                     GPIOG


#define INPUT9_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT9_GPIO_PIN                 GPIO_PIN_8
#define INPUT9_GPIO                     GPIOG


#define INPUT10_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT10_GPIO_PIN                 GPIO_PIN_9
#define INPUT10_GPIO                     GPIOG


#define INPUT11_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT11_GPIO_PIN                 GPIO_PIN_10
#define INPUT11_GPIO                     GPIOG
#define INPUT11_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */


#define INPUT12_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT12_GPIO_PIN                 GPIO_PIN_15
#define INPUT12_GPIO                     GPIOG
#define INPUT12_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */




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
void INPUT_GPIO_Init(void);
Input_State INPUT_StateRead(int port);

#endif  // __BSP_GPIO_H__


