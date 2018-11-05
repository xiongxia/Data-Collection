#ifndef __STEPMOTOR_TIM_H__
#define __STEPMOTOR_TIM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/

/* 宏定义 --------------------------------------------------------------------*/
#define STEPMOTOR_TIMx                        TIM8
#define STEPMOTOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM8_CLK_ENABLE()
#define STEPMOTOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM8_CLK_DISABLE()
#define STEPMOTOR_TIMx_IRQn                   TIM8_CC_IRQn
#define STEPMOTOR_TIMx_IRQHandler             TIM8_CC_IRQHandler

/* 第1轴 */
#define STEPMOTOR_NO1_TIM_CHANNEL_x            TIM_CHANNEL_1
#define STEPMOTOR_TIM_PUL1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOI_CLK_ENABLE()     // 输出控制脉冲给电机驱动器
#define STEPMOTOR_TIM_PUL1_PORT                GPIOI                            // 对应驱动器的PUL-（驱动器使用共阳接法）
#define STEPMOTOR_TIM_PUL1_PIN                 GPIO_PIN_5                       // 而PLU+直接接开发板的VCC

#define STEPMOTOR_DIR1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()     // 电机旋转方向控制，如果悬空不接默认正转
#define STEPMOTOR_DIR1_PORT                    GPIOD                            // 对应驱动器的DIR-（驱动器使用共阳接法）
#define STEPMOTOR_DIR1_PIN                     GPIO_PIN_3                       // 而DIR+直接接开发板的VCC

#define STEPMOTOR_ENA1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()     // 电机脱机使能控制，如果悬空不接默认使能电机
#define STEPMOTOR_ENA1_PORT                    GPIOD                            // 对应驱动器的ENA-（驱动器使用共阳接法）
#define STEPMOTOR_ENA1_PIN                     GPIO_PIN_7                       // 而ENA+直接接开发板的VCC

#define STEPMOTOR_DIR1_FORWARD()               HAL_GPIO_WritePin(STEPMOTOR_DIR1_PORT,STEPMOTOR_DIR1_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_DIR1_REVERSAL()              HAL_GPIO_WritePin(STEPMOTOR_DIR1_PORT,STEPMOTOR_DIR1_PIN,GPIO_PIN_SET)

#define STEPMOTOR_OUTPUT1_ENABLE()             HAL_GPIO_WritePin(STEPMOTOR_ENA1_PORT,STEPMOTOR_ENA1_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_OUTPUT1_DISABLE()            HAL_GPIO_WritePin(STEPMOTOR_ENA1_PORT,STEPMOTOR_ENA1_PIN,GPIO_PIN_SET)

/* 第2轴 */
#define STEPMOTOR_NO2_TIM_CHANNEL_x            TIM_CHANNEL_2
#define STEPMOTOR_TIM_PUL2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()     // 输出控制脉冲给电机驱动器
#define STEPMOTOR_TIM_PUL2_PORT                GPIOI                            // 对应驱动器的PUL-（驱动器使用共阳接法）
#define STEPMOTOR_TIM_PUL2_PIN                 GPIO_PIN_6                       // 而PLU+直接接开发板的VCC

#define STEPMOTOR_DIR2_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()     // 电机旋转方向控制，如果悬空不接默认正转
#define STEPMOTOR_DIR2_PORT                    GPIOD                            // 对应驱动器的DIR-（驱动器使用共阳接法）
#define STEPMOTOR_DIR2_PIN                     GPIO_PIN_11                       // 而DIR+直接接开发板的VCC

#define STEPMOTOR_ENA2_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()     // 电机脱机使能控制，如果悬空不接默认使能电机
#define STEPMOTOR_ENA2_PORT                    GPIOF                            // 对应驱动器的ENA-（驱动器使用共阳接法）
#define STEPMOTOR_ENA2_PIN                     GPIO_PIN_11                       // 而ENA+直接接开发板的VCC

#define STEPMOTOR_DIR2_FORWARD()               HAL_GPIO_WritePin(STEPMOTOR_DIR2_PORT,STEPMOTOR_DIR2_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_DIR2_REVERSAL()              HAL_GPIO_WritePin(STEPMOTOR_DIR2_PORT,STEPMOTOR_DIR2_PIN,GPIO_PIN_SET)

#define STEPMOTOR_OUTPUT2_ENABLE()             HAL_GPIO_WritePin(STEPMOTOR_ENA2_PORT,STEPMOTOR_ENA2_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_OUTPUT2_DISABLE()            HAL_GPIO_WritePin(STEPMOTOR_ENA2_PORT,STEPMOTOR_ENA2_PIN,GPIO_PIN_SET)

/* 第3轴*/
#define STEPMOTOR_NO3_TIM_CHANNEL_x            TIM_CHANNEL_3
#define STEPMOTOR_TIM_PUL3_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()     // 输出控制脉冲给电机控制器
#define STEPMOTOR_TIM_PUL3_PORT                GPIOI                            // 对应电机驱动器的PUL-（控制器使用共阳接法）
#define STEPMOTOR_TIM_PUL3_PIN                 GPIO_PIN_7                       // 而PLU+直接接开发板的5V(或者3.3V)

#define STEPMOTOR_DIR3_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()     // 电机旋转方向控制，如果悬空不接默认正转
#define STEPMOTOR_DIR3_PORT                    GPIOF                            // 对应电机驱动器的DIR-（控制器使用共阳接法）
#define STEPMOTOR_DIR3_PIN                     GPIO_PIN_1                      // 而DIR+直接接开发板的5V(或者3.3V)

#define STEPMOTOR_ENA3_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()     // 电机使能控制，如果悬空不接默认使能电机
#define STEPMOTOR_ENA3_PORT                    GPIOF                            // 对应电机驱动器的ENA-（控制器使用共阳接法）
#define STEPMOTOR_ENA3_PIN                     GPIO_PIN_2                       // 而ENA+直接接开发板的5V(或者3.3V) 

#define STEPMOTOR_DIR3_FORWARD()               HAL_GPIO_WritePin(STEPMOTOR_DIR3_PORT,STEPMOTOR_DIR3_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_DIR3_REVERSAL()              HAL_GPIO_WritePin(STEPMOTOR_DIR3_PORT,STEPMOTOR_DIR3_PIN,GPIO_PIN_SET)

#define STEPMOTOR_OUTPUT3_ENABLE()             HAL_GPIO_WritePin(STEPMOTOR_ENA3_PORT,STEPMOTOR_ENA3_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_OUTPUT3_DISABLE()            HAL_GPIO_WritePin(STEPMOTOR_ENA3_PORT,STEPMOTOR_ENA3_PIN,GPIO_PIN_SET)

/* 第4轴*/
#define STEPMOTOR_NO4_TIM_CHANNEL_x            TIM_CHANNEL_4
#define STEPMOTOR_TIM_PUL4_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()     // 输出控制脉冲给电机控制器
#define STEPMOTOR_TIM_PUL4_PORT                GPIOI                            // 对应电机驱动器的PUL-（控制器使用共阳接法）
#define STEPMOTOR_TIM_PUL4_PIN                 GPIO_PIN_2                       // 而PLU+直接接开发板的5V(或者3.3V)

#define STEPMOTOR_DIR4_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()     // 电机旋转方向控制，如果悬空不接默认正转
#define STEPMOTOR_DIR4_PORT                    GPIOC                            // 对应电机驱动器的DIR-（控制器使用共阳接法）
#define STEPMOTOR_DIR4_PIN                     GPIO_PIN_8                       // 而DIR+直接接开发板的5V(或者3.3V)

#define STEPMOTOR_ENA4_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()     // 电机使能控制，如果悬空不接默认使能电机
#define STEPMOTOR_ENA4_PORT                    GPIOD                            // 对应电机驱动器的ENA-（控制器使用共阳接法）
#define STEPMOTOR_ENA4_PIN                     GPIO_PIN_2                       // 而ENA+直接接开发板的5V(或者3.3V) 

#define STEPMOTOR_DIR4_FORWARD()               HAL_GPIO_WritePin(STEPMOTOR_DIR4_PORT,STEPMOTOR_DIR4_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_DIR4_REVERSAL()              HAL_GPIO_WritePin(STEPMOTOR_DIR4_PORT,STEPMOTOR_DIR4_PIN,GPIO_PIN_SET)

#define STEPMOTOR_OUTPUT4_ENABLE()             HAL_GPIO_WritePin(STEPMOTOR_ENA4_PORT,STEPMOTOR_ENA4_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_OUTPUT4_DISABLE()            HAL_GPIO_WritePin(STEPMOTOR_ENA4_PORT,STEPMOTOR_ENA4_PIN,GPIO_PIN_SET)



// 定义定时器预分频，定时器实际时钟频率为：168MHz/（STEPMOTOR_TIMx_PRESCALER+1）
#define STEPMOTOR_TIM_PRESCALER               5  // 步进电机驱动器细分设置为：   32  细分
//#define STEPMOTOR_TIM_PRESCALER               9  // 步进电机驱动器细分设置为：   16  细分
//#define STEPMOTOR_TIM_PRESCALER               19  // 步进电机驱动器细分设置为：   8  细分
//#define STEPMOTOR_TIM_PRESCALER               39  // 步进电机驱动器细分设置为：   4  细分
//#define STEPMOTOR_TIM_PRESCALER               79  // 步进电机驱动器细分设置为：   2  细分
//#define STEPMOTOR_TIM_PRESCALER               159 // 步进电机驱动器细分设置为：   1  细分


// 定义定时器周期，输出比较模式周期设置为0xFFFF
#define STEPMOTOR_TIM_PERIOD                   0xFFFF
// 定义高级定时器重复计数寄存器值
#define STEPMOTOR_TIM_REPETITIONCOUNTER       0


/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_STEPMOTOR;

/* 函数声明 ------------------------------------------------------------------*/

void STEPMOTOR_TIMx_Init(void);
void Step_Motor_Init(int id);
//void STEPMOTOR_AxisMoveRel(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

#endif	/* __STEPMOTOR_TIM_H__ */
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
