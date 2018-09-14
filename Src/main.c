/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2017-03-30
  * 功    能: RS-485实验主机
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F4Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
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

/* 扩展变量 ------------------------------------------------------------------*/
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
    uint8_t txbuf[50];
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
  
  
  /* 初始化RTC实时时钟 */
  MX_RTC_Init();
  
  RS485_USARTx_Init();
  /* 板载LED初始化 */
  LED_GPIO_Init();
    /* 板子输出初始化 */
  OUTPUT_GPIO_Init();
  
  
  
  /* 基本定时器初始化：1ms中断一次 */
  BASIC_TIMx_Init();
  
  /* 在中断模式下启动定时器 */
  HAL_TIM_Base_Start_IT(&htimx);
 
  
  printf("测试\n");
  
   
   /* 使能接收，进入中断回调函数 */
  HAL_UART_Receive_IT(&husart_debug,&aRxBuffer,1);
  HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer,1);
  HAL_UART_Receive_IT(&husartx,&aRxBuffer,1);
  
  RX_MODE();
  LED2_ON;
  
  RTC_CalendarShow();
  while (1)
  {
    if(RevDevicesData){
        Save_Data();
        RevDevicesData= 0;
    }
    if(RevCommand){
        Command_Data();  
        RevCommand= 1;
    }
   if(Sample_flag){
        
       Sample_RS485();
        Sample_flag = 0;
    }
    if(UpData_flag){
        RTC_CalendarShow();
        Get_Average();
        UpData();
        UpData_flag = 0;
    }
    if(Control_flag && frist){
    //第一分钟不控制
        Control();
        Control_flag = 0;
        
     }
    if(time_Control_flag || sys_time_Control_flag){
          
        /* 获取当前时间 */
        HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  
        /* 显示时间 */
        sprintf(start_time,"%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);
        
        //printf("时间=============%02d:%02d\n", stimestructureget.Hours, stimestructureg                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             et.Minutes);
        for(i=0;i<4;i++){
            if(delay[i].devices != 0){
                //printf("vvvvvvvvvvvv%d\n",strcmp(delay[i].start_time,start_time));
                if(strcmp(delay[i].start_time,start_time) == 0){
                    //时间到
                    strcpy(delay[i].start_time,"00:00");
                   // printf("时间=============%s\n",start_time);
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
   //232接收
  if(UartHandle->Instance == USARTx){
   
    RS232_Rx_buf[RS232_Rx_Count] = aRxBuffer;
    RS232_Rx_Count ++;
    //接收完成
    if(aRxBuffer == '#'){
        printf("RS232配置信息：接收完成\n");
        RS232_Rx_buf[RS232_Rx_Count-1] = '\0';
        RS232_Rx_Count = 0;
       // Save_Data();
       // Sensor_Cfg_Mode = 1;
         Debug_flag = 1;
         RevDevicesData = 1;
         RevComplete = 1;
      //  Sample_RS485();
    }
      if(aRxBuffer == '!'){
        printf("RS232命令信息：接收完成\n");
        RS232_Rx_buf[RS232_Rx_Count-1] = '\0';
        RS232_Rx_Count = 0;
        //Command_Data();
       // Sensor_Cfg_Mode = 1;
        Debug_flag = 1;
        RevCommand = 1;
        RevComplete = 1;
      //  Sample_RS485();
    }
       if(aRxBuffer == '?'){
        printf("RS232：接收完成\n");
        RS485_Send_Data(BHZY_Conductivity,8);
        Debug_flag = 1;
        Sample_flag = 1;
        
    }
    HAL_UART_Receive_IT(&husartx,&aRxBuffer,1);
  }
  //485接收
  else if(UartHandle->Instance == RS485_USARTx && Sample_flag == 1){
//  else if(UartHandle->Instance == USARTx){
   // printf("RS485:%02x\n",aRxBuffer);
    RS485_Rx_buf[RS485_Rx_Count] = aRxBuffer;
    RS485_Rx_Count ++;
    //printf("\n%d %d\n",aRxBuffer,Rx_Count); 
    HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer,1);
  }
  //安卓端接收
  else if(UartHandle->Instance == DEBUG_USARTx){
   // printf("adada");
   // HAL_UART_Transmit(&husartx,&aRxBuffer,1,0);
    Android_Rx_buf[Android_Rx_Count] = aRxBuffer;
    Android_Rx_Count ++;
    //接收完成
    if(aRxBuffer == '#'){
        printf("安卓配置信息：接收完成\n");
        Android_Rx_buf[Android_Rx_Count-1] = '\0';
        printf("%s\n",Android_Rx_buf);
        Android_Rx_Count = 0;
        //Save_Data();
        RevDevicesData = 1;
    }
      if(aRxBuffer == '!'){
        printf("安卓命令信息：接收完成\n");
        Android_Rx_buf[Android_Rx_Count-1] = '\0';
        printf("%s\n",Android_Rx_buf);
        Android_Rx_Count = 0;
       // Command_Data();
        RevCommand = 1;
    }
    HAL_UART_Receive_IT(&husart_debug,&aRxBuffer,1);
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
    //采集定时
    second_timer_count++;
  
    minute_timer_count++;
    
    
    //1s钟采集一次
    LED1_TOGGLE;
    if(second_timer_count == 2567)
    {
        second_timer_count = 0;
     
        LED2_TOGGLE;
      
        //有配置信息采集
        if(Sensor_Cfg_Mode){
            Sample_flag = 1;
            if(Delay_Cfg_Mode){
            // Control();
                Control_flag = 1;

            }
        }
        
    }
 
    //1分钟上传一次
     if(minute_timer_count == 60000)
     {
        
        LED3_TOGGLE;
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


/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
