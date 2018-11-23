/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usart/bsp_debug_usart.h"
#include "RS485/bsp_usartx_RS485.h"
#include "led/bsp_led.h"
#include "BasicTIM/bsp_BasicTIM.h"
#include "string.h"
#include "function.h"
#include "sample.h"
#include "gpio/bsp_gpio.h"
#include "variable.h"
#include "rtc/bsp_rtc.h"
#include "spiflash/bsp_spiflash.h"
#include <stdlib.h>
    
/* 私有类型定义 --------------------------------------------------------------*/

__IO uint16_t minute_timer_count = 0;//分
__IO uint16_t second_timer_count = 0;//秒

__IO uint16_t count = 0;

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/

uint8_t aRxBuffer = 0;
uint8_t frist = 0;
uint8_t RevDevicesData = 0;
uint8_t RevCommand = 0;

/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     //使能PWR时钟

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  //设置调压器输出电压级别1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        //打开HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    //打开PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            //PLL时钟源选择HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 //8分频MHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               //336倍频
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     //2分频，得到168MHz主时钟
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 //USB/SDIO/随机数产生器等的主PLL分频系数
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟： 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1时钟：42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2时钟：84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // 使能CSS功能，优先使用外部晶振，内部时钟源为备用
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */

int main(void)
{
  
    int i =0;
    char start_time[20];
    RTC_TimeTypeDef stimestructureget;
    
  
  /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
  HAL_Init();
  /* 配置系统时钟 */
  SystemClock_Config();
  /* 初始化串口并配置串口中断优先级 */
  MX_DEBUG_USART_Init();
  
  /* 初始化串口并配置串口中断优先级 */
  MX_USARTx_Init();
  
  MX_SPIFlash_Init();
  
  /* 初始化RTC实时时钟 */
  MX_RTC_Init();
  
  RS485_USARTx_Init();
  /* 板载LED初始化 */
  LED_GPIO_Init();
    /* 板子输出初始化 */
  OUTPUT_GPIO_Init();
  //输入初始化
  INPUT_GPIO_Init();
  /* 基本定时器初始化：1ms中断一次 */
  BASIC_TIMx_Init();
   
  Open_Light();
  printf("测试\n");
   
   /* 使能接收，进入中断回调函数 */
  HAL_UART_Receive_IT(&husart_debug,&aRxBuffer,1);
  HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer,1);
  HAL_UART_Receive_IT(&husartx,&aRxBuffer,1);
  
  RX_MODE();
  
  RTC_CalendarShow();
 
  //获取配置
  Get_Device_Data(Android_Rx_buf);
  strcpy(Rx_buf,Android_Rx_buf);
 // SCM_state = SCM_RUN;
  if(SCM_state == SCM_RUN){
    /* 在中断模式下启动定时器 */
    HAL_TIM_Base_Start_IT(&htimx);

  }
  else{
    HAL_TIM_Base_Stop(&htimx);
  }


  while (1)
  {
    
    if(Reboot_flag){
      Reboot();
      Reboot_flag = 0;
      
    }
    
    if(RevDevicesData){
      Save_Data();
      if(Debug_flag){
          Save_Device_Data(RS232_Rx_buf);
      }
      else{
          Save_Device_Data(Android_Rx_buf);
      }
        RevDevicesData= 0;
    }
    if(RevCommand){
        Command_Data();  
        RevCommand= 0;
    }
   if(Sample_flag){
       Sample_RS485();
       Sample_flag = 0;
    }
    if(UpData_flag){
        RTC_CalendarShow();
        Get_Average();
        UpData();
        Detection();
        UpData_flag = 0;
    }
    if(Control_flag && frist){
    //第一分钟不控制
        Control();
        Control_flag = 0;
        
     }
    if(Save_flag){
      Save_Device_Data(Rx_buf);
      Save_flag = 0;
    }
    if(YiYe_pump_control_flag){
      //控制移液泵
      if(YiYe_pump_flag){
        Open_Delay('3');
      }
      else{
        Close_Delay('3');
      }
      YiYe_pump_control_flag = 0;
    }
    if(time_Control_flag || sys_time_Control_flag){
          
        /* 获取当前时间 */
        HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
        sprintf(start_time,"%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
        for(i=0;i<4;i++){
            if(delay[i].devices != 0){
                if(strcmp(delay[i].start_time,start_time) == 0){
                    //时间到
                    strcpy(delay[i].start_time,"00:00");
                    if(delay[i].state == 0){
                        Open_Delay(delay[i].port);
                    }
                }

                if(delay[i].counter <= 0){
                    //计数结束
                    delay[i].counter = delay[i].save_counter;
                    if(delay[i].state == 1){
                        Close_Delay(delay[i].port);
                    }
                    if(delay[i].control == 3){
                        Chang_Start_time(i);
                    }

                }
            }
        }
    }
  }
}

/**
  * 函数功能: 串口接收完成回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  //printf("%02x\n",aRxBuffer);
  //安卓端接收
  if(UartHandle->Instance == DEBUG_USARTx){
    if(Android_Rx_Count != 0 || aRxBuffer != 0x20){
          Android_Rx_buf[Android_Rx_Count] = aRxBuffer;
          Android_Rx_Count ++;
    }
    //接收完成
    if(aRxBuffer == '#'){
        RevDevicesData = 1;
        printf("安卓配置信息：接收完成\n");
        Android_Rx_buf[Android_Rx_Count-1] = '\0';
        Android_Rx_Count = 0;
    }
    if(aRxBuffer == '!'){
        RevCommand = 1;
        printf("安卓命令信息：接收完成\n");
        Android_Rx_buf[Android_Rx_Count-1] = '\0';
        Android_Rx_Count = 0;       
    }
    //更新系统
    if(aRxBuffer == '&' && Android_Rx_Count < 5){
        printf("\n安卓测试信息：接收完成\n");
        printf("系统准备重启，更新程序\n");
        Reboot_flag = 1;
    }
    HAL_UART_Receive_IT(&husart_debug,&aRxBuffer,1);
  }
  //485接收
  else if(UartHandle->Instance == RS485_USARTx){
    RS485_Rx_buf[RS485_Rx_Count] = aRxBuffer;
    RS485_Rx_Count ++;
    HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer,1);
  }
}


/**
  * 函数功能: 非阻塞模式下定时器的回调函数
  * 输入参数: htim：定时器句柄
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    int i;
    char data[5];
    //采集定时
    second_timer_count++;
  
    minute_timer_count++;
    
    
    //1s钟采集一次
   // LED1_TOGGLE;
    if(second_timer_count == 3526)
    {
        second_timer_count = 0;    
        //有配置信息采集
        if(Sensor_Cfg_Mode){
            Sample_flag = 1;
            if(Delay_Cfg_Mode){
                Control_flag = 1;

            }
        }      
    }
    if(second_timer_count == 2054){
            //一秒发送一次心跳        
        sprintf(data,"@%d",SCM_state);
        HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);    
    }

    //1分钟上传一次
     if(minute_timer_count == 60110)
     {
       // LED3_TOGGLE;
        minute_timer_count = 0;
            //有配置信息采集
        if(Sensor_Cfg_Mode){
           UpData_flag = 1;
            if(!frist){
                frist = 1;
            }
        }
     }
    for(i=0;i<4;i++){
        if(delay[i].devices != 0 && delay[i].state == 1){
            delay[i].counter --;
        }
    }
    
}

/**
  * 函数功能: 按键外部中断服务函数
  * 输入参数: GPIO_Pin：中断引脚
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==INPUT11_GPIO_PIN)//开
  {
      LED1_ON;
      LED2_ON;
      LED3_ON;
      if(SCM_state != SCM_RUN){
        SCM_state = SCM_RUN;
         Save_flag = 1;
        HAL_TIM_Base_Start_IT(&htimx);
      }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT11_GPIO_PIN);
  }
  else if(GPIO_Pin==INPUT12_GPIO_PIN)//关
  {
      LED1_OFF;
      LED2_OFF;
      LED3_OFF;
      if(SCM_state != SCM_STOP){
        SCM_state = SCM_STOP;
         Save_flag = 1;
        HAL_TIM_Base_Stop(&htimx);
      }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT12_GPIO_PIN);
  }
  else if(GPIO_Pin==INPUT10_GPIO_PIN)//移液泵
  {
    YiYe_pump_control_flag = 1;
    if(YiYe_pump_flag == 1){
      YiYe_pump_flag = 0;
    }
    else if(YiYe_pump_flag == 0){
      YiYe_pump_flag = 1;
    }
    else{
      YiYe_pump_flag = 0;
      printf("移液泵状态错误，已关闭！\n");
    }
   
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT10_GPIO_PIN);
  }
   else if(GPIO_Pin==INPUT9_GPIO_PIN)//急停
  {
      LED1_OFF;
      LED2_OFF;
      LED3_OFF;
      if(SCM_state != SCM_STOP){
        SCM_state = SCM_STOP;
        Save_flag = 1;
        HAL_TIM_Base_Stop(&htimx);
      }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT9_GPIO_PIN);
  }
  //Save_Device_Data(Rx_buf);
  
  
}





/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
