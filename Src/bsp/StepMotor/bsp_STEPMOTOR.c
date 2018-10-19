/**
  ******************************************************************************
  * 文件名程: bsp_STEPMOTOR.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2017-05-31
  * 功    能: 步进电机驱动器控制实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "StepMotor/bsp_STEPMOTOR.h" 

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
__IO uint16_t Toggle_Pulse = 450;         // 比较输出周期，值越小输出频率越快，默认最快

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 驱动器相关GPIO初始化配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void STEPMOTOR_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct; 
  
  /* 引脚端口时钟使能 */
  STEPMOTOR_TIM_GPIO_CLK_ENABLE();
  STEPMOTOR_DIR_GPIO_CLK_ENABLE();
  STEPMOTOR_ENA_GPIO_CLK_ENABLE();
  
  /* 驱动器脉冲控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_TIM_PUL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AFx_TIMx;        // GPIO引脚用做TIM复用功能
  HAL_GPIO_Init(STEPMOTOR_TIM_PUL_PORT, &GPIO_InitStruct);
  
  /* 驱动器方向控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_DIR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_DIR_PORT, &GPIO_InitStruct);
  
  /* 驱动器脱机使能控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_ENA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_ENA_PORT, &GPIO_InitStruct);
  
  STEPMOTOR_DIR_FORWARD();
  STEPMOTOR_OUTPUT_ENABLE();
}

/**
  * 函数功能: 驱动器定时器初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void STEPMOTOR_TIMx_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;             // 定时器时钟
  TIM_MasterConfigTypeDef sMasterConfig;                 // 定时器主模式配置
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;   // 刹车和死区时间配置
  TIM_OC_InitTypeDef sConfigOC;                          // 定时器通道比较输出
  
  /* 定时器基本环境配置 */
  htimx_STEPMOTOR.Instance = STEPMOTOR_TIMx;                                 // 定时器编号
  htimx_STEPMOTOR.Init.Prescaler = STEPMOTOR_TIM_PRESCALER;                  // 定时器预分频器
  htimx_STEPMOTOR.Init.CounterMode = TIM_COUNTERMODE_UP;                     // 计数方向：向上计数
  htimx_STEPMOTOR.Init.Period = STEPMOTOR_TIM_PERIOD;                        // 定时器周期
  htimx_STEPMOTOR.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;                 // 时钟分频
  htimx_STEPMOTOR.Init.RepetitionCounter = STEPMOTOR_TIM_REPETITIONCOUNTER;  // 重复计数                                                                         器
  HAL_TIM_Base_Init(&htimx_STEPMOTOR);

  /* 定时器时钟源配置 */
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;                 // 使用内部时钟源
  HAL_TIM_ConfigClockSource(&htimx_STEPMOTOR, &sClockSourceConfig);

  /* 初始化定时器比较输出环境 */
  HAL_TIM_OC_Init(&htimx_STEPMOTOR);
  
  /* 定时器主输出模式 */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimx_STEPMOTOR, &sMasterConfig);
  
  /* 刹车和死区时间配置 */
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&htimx_STEPMOTOR, &sBreakDeadTimeConfig);

  /* 定时器比较输出配置 */
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                // 比较输出模式：反转输出
  sConfigOC.Pulse = Toggle_Pulse;                      // 脉冲数
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;          // 输出极性
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;         // 互补通道输出极性
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;           // 快速模式
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;       // 空闲电平
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;     // 互补通道空闲电平
  HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_TIM_CHANNEL_x);

  /* STEPMOTOR相关GPIO初始化配置 */
  STEPMOTOR_GPIO_Init();
  
  /* 配置定时器中断优先级并使能 */
  HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);

}


/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
