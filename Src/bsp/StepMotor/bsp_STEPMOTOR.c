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
#include "variable.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
//__IO uint16_t Toggle_Pulse = 450;         // 比较输出周期，值越小输出频率越快，默认最快

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
  STEPMOTOR_TIM_PUL1_GPIO_CLK_ENABLE();
  STEPMOTOR_DIR1_GPIO_CLK_ENABLE();
  STEPMOTOR_ENA1_GPIO_CLK_ENABLE();
  
  STEPMOTOR_TIM_PUL2_GPIO_CLK_ENABLE();
  STEPMOTOR_DIR2_GPIO_CLK_ENABLE();
  STEPMOTOR_ENA2_GPIO_CLK_ENABLE();
  
  STEPMOTOR_TIM_PUL3_GPIO_CLK_ENABLE();
  STEPMOTOR_DIR3_GPIO_CLK_ENABLE();
  STEPMOTOR_ENA3_GPIO_CLK_ENABLE();
  
  STEPMOTOR_TIM_PUL4_GPIO_CLK_ENABLE();
  STEPMOTOR_DIR4_GPIO_CLK_ENABLE();
  STEPMOTOR_ENA4_GPIO_CLK_ENABLE();
  
  /* 第1轴*/
  /* 驱动器脉冲控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_TIM_PUL1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;        // GPIO引脚用做TIM复用功能
  HAL_GPIO_Init(STEPMOTOR_TIM_PUL1_PORT, &GPIO_InitStruct);
  
  /* 驱动器方向控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_DIR1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_DIR1_PORT, &GPIO_InitStruct);
  
  /* 驱动器脱机使能控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_ENA1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_ENA1_PORT, &GPIO_InitStruct);
  
  /* 第2轴*/
  /* 电机驱动器输出脉冲控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_TIM_PUL2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;        // GPIO引脚用做TIM复用功能
  HAL_GPIO_Init(STEPMOTOR_TIM_PUL2_PORT, &GPIO_InitStruct);
  
  /* 电机驱动器方向控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_DIR2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_DIR2_PORT, &GPIO_InitStruct);
  
  /* 电机驱动器使能控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_ENA2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_ENA2_PORT, &GPIO_InitStruct);
  
  /* 第3轴*/
  /* 电机驱动器输出脉冲控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_TIM_PUL3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;        // GPIO引脚用做TIM复用功能
  HAL_GPIO_Init(STEPMOTOR_TIM_PUL3_PORT, &GPIO_InitStruct);
  
  /* 电机驱动器方向控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_DIR3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_DIR3_PORT, &GPIO_InitStruct);
  
  /* 电机驱动器使能控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_ENA3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_ENA3_PORT, &GPIO_InitStruct);
  
  /* 第4轴*/
  /* 电机驱动器输出脉冲控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_TIM_PUL4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;        // GPIO引脚用做TIM复用功能
  HAL_GPIO_Init(STEPMOTOR_TIM_PUL4_PORT, &GPIO_InitStruct);
  
  /* 电机驱动器方向控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_DIR4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_DIR4_PORT, &GPIO_InitStruct);
  
  /* 电机驱动器使能控制引脚IO初始化 */
  GPIO_InitStruct.Pin = STEPMOTOR_ENA4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;       // GPIO引脚用做系统默认功能
  HAL_GPIO_Init(STEPMOTOR_ENA4_PORT, &GPIO_InitStruct);
  
  STEPMOTOR_DIR1_FORWARD();
  STEPMOTOR_OUTPUT1_ENABLE();
  
  STEPMOTOR_DIR2_FORWARD();
  STEPMOTOR_OUTPUT2_ENABLE();
  
  STEPMOTOR_DIR3_FORWARD();
  STEPMOTOR_OUTPUT3_ENABLE();
  
  STEPMOTOR_DIR4_FORWARD();
  STEPMOTOR_OUTPUT4_ENABLE();
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
  htimx_STEPMOTOR.Init.RepetitionCounter = STEPMOTOR_TIM_REPETITIONCOUNTER;  // 重复计数器
  HAL_TIM_Base_Init(&htimx_STEPMOTOR);

  /* 定时器时钟源配置 */
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       // 使用内部时钟源
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

  Step_Motor_Init(5);
  
  /* 定时器比较输出配置 */
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                // 比较输出模式：反转输出
  sConfigOC.Pulse = Stepper_Motor[0].Toggle_Pulse;                   // 脉冲数
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;          // 输出极性
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;         // 互补通道输出极性
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;           // 快速模式
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;       // 空闲电平
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;     // 互补通道空闲电平
  HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_NO1_TIM_CHANNEL_x);

  sConfigOC.Pulse = Stepper_Motor[1].Toggle_Pulse;                   // 脉冲数
  HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_NO2_TIM_CHANNEL_x);
  
  sConfigOC.Pulse = Stepper_Motor[2].Toggle_Pulse;                   // 脉冲数
  HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_NO3_TIM_CHANNEL_x);
  
  sConfigOC.Pulse = Stepper_Motor[3].Toggle_Pulse;                  // 脉冲数
  HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_NO4_TIM_CHANNEL_x);
  
  /* STEPMOTOR相关GPIO初始化配置 */
  STEPMOTOR_GPIO_Init();
  
  /* 配置定时器中断优先级并使能 */
  HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);

}
/**
  * 函数功能: 电机参数初始化
  * 输入参数: 对应 id
  * 返 回 值: 无
  * 说    明: id == 5 全部初始化
typedef struct {
  __IO uint8_t  run_state ;  // 电机旋转状态
  __IO uint8_t  dir ;        // 电机旋转方向
  __IO int32_t  step_delay;  // 下个脉冲周期（时间间隔），启动时为加速度
  __IO uint32_t decel_start; // 启动减速位置
  __IO int32_t  decel_val;   // 减速阶段步数
  __IO int32_t  min_delay;   // 最小脉冲周期(最大速度，即匀速段速度)
  __IO int32_t  accel_count; // 加减速阶段计数值
  __IO int32_t  speed; // 匀速速度（rpm）
  __IO int32_t  step; //步数
  __IO int32_t  min_speed; //最小速度  （根据电机不同有所不同）
  __IO int32_t  max_speed; //最大速度
  __IO uint16_t Toggle_Pulse;//脉冲频率
}speedRampData;
  */
void Step_Motor_Init(int id){
  int i;
  if(id == 5){
    for(i=0;i<4;i++){
      Stepper_Motor[i].run_state = STOP;
      Stepper_Motor[i].dir = CW;
      Stepper_Motor[i].step_delay = 0;
      Stepper_Motor[i].decel_start = 0;
      Stepper_Motor[i].decel_val = 0;
      Stepper_Motor[i].min_delay = 0;
      Stepper_Motor[i].accel_count = 0;
      Stepper_Motor[i].speed = 0;
      Stepper_Motor[i].min_speed = 0;
      Stepper_Motor[i].max_speed = 0;
      Stepper_Motor[i].Toggle_Pulse = 1000;
      Stepper_Motor[i].step = 0;
    }
  }
  else{
      Stepper_Motor[id-1].run_state = STOP;
      Stepper_Motor[id-1].dir = CW;
      Stepper_Motor[id-1].step_delay = 0;
      Stepper_Motor[id-1].decel_start = 0;
      Stepper_Motor[id-1].decel_val = 0;
      Stepper_Motor[id-1].min_delay = 0;
      Stepper_Motor[id-1].accel_count = 0;
      Stepper_Motor[id-1].speed = 0;
      Stepper_Motor[id-1].min_speed = 0;
      Stepper_Motor[id-1].max_speed = 0;
      Stepper_Motor[id-1].Toggle_Pulse = 800;
      Stepper_Motor[id-1].step = 0;
  
  }

}



/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
