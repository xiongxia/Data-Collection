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
uint8_t error = 0;
uint8_t error_num = 0;

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
uint8_t debug_test = 1;
uint8_t aRxBuffer= 0;
uint8_t aRxBuffer_485 = 0;
uint8_t frist = 0;
uint8_t RevDevicesData = 0;
uint8_t RevCommand = 0;
uint8_t com_flag = 0;
uint8_t ret = HAL_OK;
uint8_t Rx_Count = 0;
uint8_t aRxBuffer_Android = 0;
/* 私有函数原形 --------------------------------------------------------------*/
void SystemClock_Config(void);
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */

int main(void)
{
  
    int i = 0;
    char start_time[20];
    char info[5];
    RTC_TimeTypeDef stimestructureget;
    
  /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
  HAL_Init();
  /* 配置系统时钟 */
  SystemClock_Config();

  /* 初始化串口并配置串口中断优先级 */
 // MX_USARTx_Init();
  
  MX_SPIFlash_Init();
  MX_USARTx_Init();
  
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
  MX_DEBUG_USART_Init();
  printf("远创新智——测试3.8\n");
    
     /* 使能接收，进入中断回调函数 */
  HAL_UART_Receive_IT(&husart_debug,&aRxBuffer_Android,1);
  HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer_485,1);

  
  RX_MODE();
  RTC_CalendarShow();
  sprintf(info,"|2",SCM_state);
  HAL_UART_Transmit(&husart_debug,info,strlen((char *)info),1000); 
  
  Close_Delay('1');
  Close_Delay('2');
  Close_Delay('3');
  Close_Delay('4');
  Close_Delay('5');
  Close_YiYe_pupm();
  Close_Light();
  Close_Beep();
  SCM_state = SCM_RUN;
  
  //获取配置
  Get_Device_Data(Android_Rx_buf);
  //strcpy(Rx_buf,Android_Rx_buf);

  if(SCM_state == SCM_RUN)
  {
    Open_Light();
    LED1_ON;
    /* 在中断模式下启动定时器 */
    HAL_TIM_Base_Start_IT(&htimx);
  }
  else
  {
    Close_Light();
    HAL_TIM_Base_Stop(&htimx);
  }

  while (1)
  {
    //采集指令
    if(Sample_flag)
    {
       LED2_TOGGLE;
       Sample_RS485();
       Sample_flag = 0;
    }
    //接收到安卓的配置信息
    if(RevDevicesData)
    {
      RTC_CalendarShow();
      printf("安卓：%s\n",Android_Rx_buf);
      frist = 0;
      Sample_flag = 0;
      second_timer_count = 0;
      minute_timer_count = 0;;
      time_num = 0;
      Save_Data();
      FLAG = 0;
      RevDevicesData= 0;
    }
    //接收到安卓的命令信息
    if(RevCommand)
    {
        RTC_CalendarShow();
        Command_Data();  
        RevCommand= 0;
        FLAG = 0;
    }
      //接收到重启指令
    if(Reboot_flag)
    {
      RTC_CalendarShow();
      FLAG = 1;
      Close_Delay('1');
      Close_Delay('2');
      Close_Delay('3');
      Close_Delay('4');
      Close_Delay('5');
      Close_YiYe_pupm();
      Close_Light();
      Close_Beep();
      if(!error)
        HAL_UART_Transmit(&husart_debug,"&",1,1000);
      FLAG = 0;
      Reboot();
      Reboot_flag = 0; 
    }

    //上传数据
    if(UpData_flag)
    {
        RTC_CalendarShow();
        FLAG = 1;
        LED3_TOGGLE;
        //RTC_CalendarShow();
        Get_Average();
        UpData();
        Detection();
        UpData_flag = 0;
        FLAG = 0;
    }
    if(Control_flag && frist)
    {
        FLAG = 1;
        //第一分钟不控制
        printf("开始控制逻辑\n");
        LED3_TOGGLE;
        Control();
        Control_flag = 0;
        FLAG = 0;
    }
    //更新时间
    if(RTC_Config_flag)
    {
       RTC_CalendarConfig(Android_Rx_buf);
       RTC_CalendarShow();
       RTC_Config_flag = 0;
       FLAG = 0;
    }

    //停止后重启，恢复状态
    if(Save_flag)
    {
      RTC_CalendarShow();
      FLAG = 1;
      if(SCM_state == SCM_RUN)
      {
          Open_Light();
          for(i=0;i<5;i++)
          {
            if(delay[i].devices != 0 && delay[i].state == 1)
            {
              Open_Delay(delay[i].port);
            }
          }
          if(YiYe_pump_flag)
          {
            Open_YiYe_pupm();
          }
      }
      else
      {
        Close_Light();
        Close_Delay('1');
        Close_Delay('2');
        Close_Delay('3');
        Close_Delay('4');
        Close_Delay('5');
        Close_YiYe_pupm();
        Close_Light();
        Close_Beep();
      }
      Save_flag = 0;
      FLAG = 0;
    }
    if(YiYe_pump_control_flag)
    {
      //控制移液泵
      if(YiYe_pump_flag)
      {
        Open_YiYe_pupm();
      }
      else
      {
        Close_YiYe_pupm();
      }
      YiYe_pump_control_flag = 0;
    }
    if(Error_flag)
    {
      //解除告警
      Close_Error(Android_Rx_buf[0]);
      Error_flag = 0;
      FLAG = 0;
    }
    if(clear_flag)
    {
      Clear_FLASH_Data();
      clear_flag = 0;
    }
    //根据时间控制继电器
    if(time_Control_flag || sys_time_Control_flag)
    {
         //FLAG = 1;
        /* 获取当前时间 */
        HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
        sprintf(start_time,"%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
        for(i=0;i<5;i++)
        {
            if(delay[i].devices != 0 && delay[i].control > 2)
            {
                if(strcmp(delay[i].start_time,start_time) == 0)
                {
                    printf("计时开始，打开泵%d\n",i);
                    //时间到
                    strcpy(delay[i].start_time,"00:00");
                    Open_Delay(delay[i].port);
                }

                if(delay[i].counter <= 10 && delay[i].state == 1)
                {
                    //计数结束
                    printf("计时结束，关闭泵%d\n",i);
                    delay[i].counter = delay[i].save_counter;
                    Close_Delay(delay[i].port);
                    if(delay[i].control == 3)
                    {
                        Chang_Start_time(i);
                    }
                    else if(delay[i].control == 6){
     
                      printf("继电器关闭，模式6控制方式，还需要%d\n",time_num);
 
                    }
                }
            }
        }
        //FLAG = 0;
    }
      //数据太多清除缓存
   if(Rx_Count >= 200 ){
      Rx_Count = 0;
      FLAG = 0;
    }
    if(Android_Rx_Count >= 900) 
    {
       Android_Rx_Count = 0;
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
  
  //安卓端接收
  if(UartHandle->Instance == DEBUG_USARTx){
    
    if(FLAG){
      printf("安卓接收但不保存数据：%c\n",aRxBuffer);
      HAL_UART_Receive_IT(&husart_debug,&aRxBuffer,1);
      Rx_Count++;
      Android_Rx_Count = 0;
      if(aRxBuffer == '{'){
        FLAG = 0;
        aRxBuffer_Android = '{';
      }
    }
    else{
      printf("安卓接收保存数据：%c\n",aRxBuffer_Android);
      if(Android_Rx_Count != 0 || aRxBuffer_Android != 0x20){
        Android_Rx_buf[Android_Rx_Count] = aRxBuffer_Android;
        Android_Rx_Count++;
      }
      switch(aRxBuffer_Android){
        case '#': 
          FLAG = 1;
          RevDevicesData = 1;
          Android_Rx_buf[Android_Rx_Count - 1] = '\0';
          printf("安卓配置信息：接收完成\n");
          Android_Rx_Count = 0;
          break;
        case '!': 
          FLAG = 1;
          RevCommand = 1;
          Android_Rx_buf[Android_Rx_Count - 1] = '\0';
          printf("安卓命令信息：接收完成\n");
          Android_Rx_Count = 0;
          break;
       //解除警告
        case '$':
          if(Android_Rx_Count < 5)
          {
            FLAG = 1;
            Android_Rx_buf[Android_Rx_Count - 1] = '\0';
            Error_flag = 1;
            printf("安卓解除信息：接收完成\n");
            Android_Rx_Count = 0;
          }
          break;
        case ':' :
          if(Android_Rx_Count <= 50 && Android_Rx_Count >= 10)
          {
            FLAG = 1;
            Android_Rx_buf[Android_Rx_Count - 1] = '\0';
            RTC_Config_flag = 1;
            printf("安卓更新时间信息：接收完成\n");      
            Android_Rx_Count = 0;
          }
          break;
        case '{':
        //通信
          HAL_UART_Transmit(&husart_debug,"{",1,1000);
          com_flag = 1;
          Android_Rx_buf[Android_Rx_Count - 1] = '\0';       
          printf("安卓通信信息：接收完成\n");
          Android_Rx_Count = 0;
          break;
      case '&':
        //更新系统
        if(Android_Rx_Count < 5)
        {
          FLAG = 1;
          printf("系统准备重启，更新程序\n");
          Reboot_flag = 1;
        }
        break;
      case '@':
        if(Android_Rx_Count < 5)
        {
          clear_flag = 1;
          printf("安卓清除单片机FLASH接收完成\n\n");
          Android_Rx_Count = 0;
        }
        break;
      default:
        break;
     }

     do{
          if(UartHandle->RxState == HAL_UART_STATE_READY){
            HAL_UART_Receive_IT(&husart_debug,&aRxBuffer_Android,1);
            break;
          }
          error_num++;
          if(error_num >=10){
            error_num = 0;
            printf("串口堵塞，自动重启\n");
            //HAL_UART_Receive(&husart_debug,(uint8_t *)aRxBuffer_Android,500,1000);
            Reboot_flag = 1;
            error = 1;
            break;
          }
     }while(1);
    }
    
  }
  //485接收
  else if(UartHandle->Instance == RS485_USARTx)
  {
    //printf("\n485:%02x\n",aRxBuffer);
    RS485_Rx_buf[RS485_Rx_Count] = aRxBuffer_485;
    RS485_Rx_Count ++;
    do{
          if(UartHandle->RxState == HAL_UART_STATE_READY){
            HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer_485,1);
            break;
          }
          error_num++;
          if(error_num >=10){
            error_num = 0;
            printf("485串口堵塞，自动重启\n");
            Reboot_flag = 1;
            error = 1;
            break;
          }
     }while(1);
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
    time_num++;
    
    //1s钟采集一次
    if(second_timer_count % 3561 == 3560)
    {
        //有配置信息采集
        if(Sensor_Cfg_Mode)
        {
            Sample_flag = 1;
        }      
    }
    if(second_timer_count == 10056)
    {
        second_timer_count = 0; 
        //10秒发送一次心跳        
        sprintf(data,"@%d",SCM_state);
        HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000); 
        printf("发送心跳\n");
    }
    
    if(Delay_Cfg_Mode && minute_timer_count == 9145)
    {
            Control_flag = 1;
            
    }
    //1分钟上传一次
     if(minute_timer_count == 60110)
     {
       // LED3_TOGGLE;
        minute_timer_count = 0;
            //有配置信息采集
        if(Sensor_Cfg_Mode)
        {
           UpData_flag = 1;
            if(!frist)
            {
                frist = 1;
            }
        }
     }
     for(i=0;i<5;i++)
     {
        if(delay[i].devices != 0 && delay[i].control >= 3 && delay[i].state == 1)
        {
            delay[i].counter --;
        }
     }
    if(time_num == 60000 * 30)
    {
       if(com_flag)
       {
         com_flag = 0;
       }
       else
       {
          printf("长时间未收到安卓通信消息，自动重启\n");
          Reboot_flag = 1;
          error = 1;
       }
       time_num = 0;
       printf("30分钟计时结束，重新开始模式6控制方式\n");
       index_time_control_flag = 0;
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
  
  if(GPIO_Pin == INPUT11_GPIO_PIN)//开
  {
      LED1_ON;
      if(SCM_state != SCM_RUN){
        SCM_state = SCM_RUN;
        Save_flag = 1;
        HAL_TIM_Base_Start_IT(&htimx);
      }
      
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT11_GPIO_PIN);
  }
  else if(GPIO_Pin == INPUT12_GPIO_PIN)//关
  {
      LED1_OFF;
      if(SCM_state != SCM_STOP){
        SCM_state = SCM_STOP;
        Save_flag = 1;
        HAL_TIM_Base_Stop(&htimx);
      }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT12_GPIO_PIN);
  }
  else if(GPIO_Pin == INPUT10_GPIO_PIN)//移液泵
  {
    YiYe_pump_control_flag = 1;
    YiYe_pump_flag = YiYe_pump_flag ^ 0xFE;
   
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT10_GPIO_PIN);
  }
   else if(GPIO_Pin == INPUT9_GPIO_PIN)//急停
  {
      LED1_OFF;
      if(SCM_state != SCM_STOP){
        SCM_state = SCM_STOP;
        Save_flag = 1;
        HAL_TIM_Base_Stop(&htimx);
      }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT9_GPIO_PIN);
  }
  
  
}
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


