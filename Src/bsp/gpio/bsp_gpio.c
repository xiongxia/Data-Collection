/**
  ******************************************************************************
  * 文件名程: bsp_gpio.c 
  * 作    者: 
  * 版    本: V1.0
  * 编写日期: 2017-3-30
  * 功    能: GPIO-输入检测
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "gpio/bsp_gpio.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 板载按键IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_key.h
  *           文件相关宏定义就可以方便修改引脚。
    
    9,10,11和12是中断模式用于按键，其他是轮询模式
  */
void INPUT_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 使能(开启)INPUT引脚对应IO端口时钟 */  
  INPUT1_RCC_CLK_ENABLE();
  INPUT2_RCC_CLK_ENABLE();
  INPUT3_RCC_CLK_ENABLE();
  INPUT4_RCC_CLK_ENABLE();
  INPUT5_RCC_CLK_ENABLE();
  INPUT6_RCC_CLK_ENABLE();
  INPUT7_RCC_CLK_ENABLE();
  INPUT8_RCC_CLK_ENABLE();
  INPUT9_RCC_CLK_ENABLE();
  INPUT10_RCC_CLK_ENABLE();
  INPUT11_RCC_CLK_ENABLE();
  INPUT12_RCC_CLK_ENABLE();

  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT1_GPIO, &GPIO_InitStruct);   
  
  GPIO_InitStruct.Pin = INPUT2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT2_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT3_GPIO_PIN;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT3_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT4_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT4_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT5_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT5_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT6_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT6_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT7_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT7_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT8_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT8_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT9_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT9_GPIO, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = INPUT10_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT10_GPIO, &GPIO_InitStruct);  
  
  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT11_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT11_GPIO, &GPIO_InitStruct);   
  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT12_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT12_GPIO, &GPIO_InitStruct);   
  
    /* 配置中断优先级 */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn,0,1);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    /* 配置中断优先级 */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  
}
/**
  * 函数功能: 读取输入状态
  * 输入参数：对应端口
  * 返 回 值: High：输入高电平；
  *           Low  ：输入低电平
  * 说    明：无
  */
Input_State INPUT_StateRead(int port)
{
  GPIO_TypeDef* INPUT_GPIO;
  uint16_t INPUT_GPIO_PIN;
  switch (port){
    case 1:
      INPUT_GPIO = INPUT1_GPIO;
      INPUT_GPIO_PIN = INPUT2_GPIO_PIN;
      break;
    case 2:
      INPUT_GPIO = INPUT2_GPIO;
      INPUT_GPIO_PIN = INPUT2_GPIO_PIN;
      break;     
    case 3:
      INPUT_GPIO = INPUT3_GPIO;
      INPUT_GPIO_PIN = INPUT3_GPIO_PIN;
      break;  
    case 4:
      INPUT_GPIO = INPUT4_GPIO;
      INPUT_GPIO_PIN = INPUT4_GPIO_PIN;
      break;  
    case 5:
      INPUT_GPIO = INPUT5_GPIO;
      INPUT_GPIO_PIN = INPUT5_GPIO_PIN;
      break;  
    case 6:
      INPUT_GPIO = INPUT6_GPIO;
      INPUT_GPIO_PIN = INPUT6_GPIO_PIN;
      break;       
    case 7:
      INPUT_GPIO = INPUT7_GPIO;
      INPUT_GPIO_PIN = INPUT7_GPIO_PIN;
      break;       
    case 8:
      INPUT_GPIO = INPUT8_GPIO;
      INPUT_GPIO_PIN = INPUT8_GPIO_PIN;
      break;      
    case 9:
      INPUT_GPIO = INPUT9_GPIO;
      INPUT_GPIO_PIN = INPUT9_GPIO_PIN;
      break;        
    case 10:
      INPUT_GPIO = INPUT10_GPIO;
      INPUT_GPIO_PIN = INPUT10_GPIO_PIN;
      break;      
    case 11:
      INPUT_GPIO = INPUT11_GPIO;
      INPUT_GPIO_PIN = INPUT11_GPIO_PIN;
      break;   
    case 12:
      INPUT_GPIO = INPUT12_GPIO;
      INPUT_GPIO_PIN = INPUT12_GPIO_PIN;
      break;  
    default:
      break;
  }
  /* 读取此时按键值并判断是否是被按下状态，如果是被按下状态进入函数内 */
  if(HAL_GPIO_ReadPin(INPUT_GPIO,INPUT_GPIO_PIN)==INPUT_DOWN_LEVEL)
  {
    /* 延时一小段时间，消除抖动 */
    HAL_Delay(20);
    /* 延时时间后再来判断按键状态，如果还是按下状态说明按键确实被按下 */
    if(HAL_GPIO_ReadPin(INPUT_GPIO,INPUT_GPIO_PIN)==INPUT_DOWN_LEVEL)
    {
      /* 等待按键弹开才退出按键扫描函数 */
      //while(HAL_GPIO_ReadPin(INPUT_GPIO,INPUT_GPIO_PIN)==INPUT_DOWN_LEVEL);      
       /* 按键扫描完毕，确定按键被按下，返回按键被按下状态 */
      return High;
    }
  }
  /* 按键没被按下，返回没被按下状态 */
  return Low;
}



/**
  * 函数功能: 板载按键IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_gpio.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void OUTPUT_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 使能(开启)KEY引脚对应IO端口时钟 */  
  OUTPUT1_RCC_CLK_ENABLE();  
  OUTPUT2_RCC_CLK_ENABLE(); 
  OUTPUT3_RCC_CLK_ENABLE(); 
  OUTPUT4_RCC_CLK_ENABLE(); 
  
  /* 配置OUTPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = OUTPUT1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT1_GPIO, &GPIO_InitStruct);   
  
  GPIO_InitStruct.Pin = OUTPUT2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT2_GPIO, &GPIO_InitStruct);   
  
  GPIO_InitStruct.Pin = OUTPUT3_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT3_GPIO, &GPIO_InitStruct);   
  
  
  GPIO_InitStruct.Pin = OUTPUT4_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT4_GPIO, &GPIO_InitStruct);   
  
  
}


