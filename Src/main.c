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
#include "spiflash/bsp_spiflash.h"
#include "StepMotor/bsp_STEPMOTOR.h"
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
__IO uint32_t pulse_count = 0; /*  脉冲计数，一个完整的脉冲会增加2 */
/* 扩展变量 ------------------------------------------------------------------*/

/*
*    当步进电机驱动器细分设置为1时，每200个脉冲步进电机旋转一周
*                          为32时，每6400个脉冲步进电机旋转一周
*    下面以设置为32时为例讲解：
*    pulse_count用于记录输出脉冲数量，pulse_count为脉冲数的两倍，
*    比如当pulse_count=12800时，实际输出6400个完整脉冲。
*    这样可以非常方便步进电机的实际转动圈数，就任意角度都有办法控制输出。
*    如果步进电机驱动器的细分设置为其它值，pulse_count也要做相应处理
*
*/

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
  /* 高级控制定时器初始化并配置PWM输出功能 */
  STEPMOTOR_TIMx_Init();
  /* 确定定时器 */
  HAL_TIM_Base_Start(&htimx_STEPMOTOR);
  
 /* 使能中断 关闭比较输出*/
  HAL_TIM_OC_Start_IT(&htimx_STEPMOTOR,STEPMOTOR_NO1_TIM_CHANNEL_x);
  HAL_TIM_OC_Start_IT(&htimx_STEPMOTOR,STEPMOTOR_NO2_TIM_CHANNEL_x);
  HAL_TIM_OC_Start_IT(&htimx_STEPMOTOR,STEPMOTOR_NO3_TIM_CHANNEL_x);
  HAL_TIM_OC_Start_IT(&htimx_STEPMOTOR,STEPMOTOR_NO4_TIM_CHANNEL_x);
  TIM_CCxChannelCmd(STEPMOTOR_TIMx,STEPMOTOR_NO1_TIM_CHANNEL_x,TIM_CCx_DISABLE);
  TIM_CCxChannelCmd(STEPMOTOR_TIMx,STEPMOTOR_NO2_TIM_CHANNEL_x,TIM_CCx_DISABLE);
  TIM_CCxChannelCmd(STEPMOTOR_TIMx,STEPMOTOR_NO3_TIM_CHANNEL_x,TIM_CCx_DISABLE);
  TIM_CCxChannelCmd(STEPMOTOR_TIMx,STEPMOTOR_NO4_TIM_CHANNEL_x,TIM_CCx_DISABLE);
  
  /* 基本定时器初始化：1ms中断一次 */
  BASIC_TIMx_Init();
  

  
  printf("测试\n");
  
   
   /* 使能接收，进入中断回调函数 */
  HAL_UART_Receive_IT(&husart_debug,&aRxBuffer,1);
  HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer,1);
  HAL_UART_Receive_IT(&husartx,&aRxBuffer,1);
  
  RX_MODE();
  LED2_ON;
  
  RTC_CalendarShow();
 
  //获取配置
  Get_Device_Data(Android_Rx_buf);
  strcpy(Rx_buf,Android_Rx_buf);
  if(SCM_state == SCM_RUN){
    /* 在中断模式下启动定时器 */
    HAL_TIM_Base_Start_IT(&htimx);

  }
  else{
    HAL_TIM_Base_Stop(&htimx);
  }
  Open_Motor(0);
  
  while (1)
  {
    
    if(RevDevicesData){
      if(Debug_flag){
            Save_Device_Data(RS232_Rx_buf);
      }
      else{
        
            Save_Device_Data(Android_Rx_buf);
      }
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
        //Get_Device_Data(Android_Rx_buf);
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
    //printf("232  %c",aRxBuffer);
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
        Reversed_Motor();
        //Command_Data();
       // Sensor_Cfg_Mode = 1;
        Debug_flag = 1;
        RevCommand = 1;
        RevComplete = 1;
      //  Sample_RS485();
    }
    //预留测试使用
       if(aRxBuffer == '?'){
        printf("\nRS232：接收完成\n");
        printf("%s  %d\n",RS232_Rx_buf,strlen(RS232_Rx_buf));
        if(strlen(RS232_Rx_buf) == 1){
          Close_Motor(1);
        }
        float f = strtod(RS232_Rx_buf,NULL);
        printf("%.2f\n",f);
       // SetSpeed(f);
        int id = (int)f;
        Open_Motor(id);
        RS232_Rx_buf[RS232_Rx_Count-1] = '\0';
        RS232_Rx_Count = 0;

        //RS485_Send_Data(BHZY_Conductivity,8);
        //Debug_flag = 1;
        //Sample_flag = 1;*/
        
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

    if(Android_Rx_Count != 0 || aRxBuffer != 0x20){
          Android_Rx_buf[Android_Rx_Count] = aRxBuffer;
          //printf("aaaaaaaaaa\n%d  %c  %02x\n",Android_Rx_Count,aRxBuffer,aRxBuffer);
          Android_Rx_Count ++;
    }
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
    //预留测试使用
    if(aRxBuffer == '?'){
        printf("\n安卓测试信息：接收完成\n");
        printf("%s  %d\n",Android_Rx_buf,strlen(Android_Rx_buf));
        if(strlen(Android_Rx_buf) == 1){
          Close_Motor(1);
        }
        float f = strtod(Android_Rx_buf,NULL);
        printf("%.2f\n",f);
        //SetSpeed(f);
        int id = (int)f;
        Open_Motor(id);
        Android_Rx_buf[RS232_Rx_Count-1] = '\0';
        Android_Rx_Count = 0;

        //RS485_Send_Data(BHZY_Conductivity,8);
        //Debug_flag = 1;
        //Sample_flag = 1;*/
        
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
    char data[5];
    //采集定时
    second_timer_count++;
  
    minute_timer_count++;
    
    
    //1s钟采集一次
   // LED1_TOGGLE;
    if(second_timer_count == 2567)
    {
        second_timer_count = 0;
     
        //LED2_TOGGLE;
      
        //有配置信息采集
        if(Sensor_Cfg_Mode){
            Sample_flag = 1;
            if(Delay_Cfg_Mode){
            // Control();
                Control_flag = 1;

            }
        }
        //一秒发送一次心跳
        
       /* printf("发送心跳\n");
        sprintf(data,"@%d",SCM_state);
        HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
*/
        
    }
                   
 
    //1分钟上传一次
     if(minute_timer_count == 60000)
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
  * 函数功能: 定时器比较输出中断回调函数
  * 输入参数: htim：定时器句柄指针
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  __IO uint32_t count;
  //__IO uint32_t tmp;
  count =__HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
  if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
  {
    __HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_NO1_TIM_CHANNEL_x,count+Stepper_Motor[0].Toggle_Pulse);
    Stepper_Motor[0].step++;
  }
  if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
  {
    __HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_NO2_TIM_CHANNEL_x,count+Stepper_Motor[1].Toggle_Pulse);
    Stepper_Motor[1].step++;
  }
  if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
  {
    __HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_NO3_TIM_CHANNEL_x,count+Stepper_Motor[2].Toggle_Pulse);
    Stepper_Motor[2].step++;
  }
  if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
  {
    __HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_NO4_TIM_CHANNEL_x,count+Stepper_Motor[3].Toggle_Pulse);
    Stepper_Motor[3].step++;
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
    HAL_Delay(20);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(INPUT11_GPIO,INPUT11_GPIO_PIN)==INPUT11_DOWN_LEVEL)
    {
      
      LED1_ON;
      LED2_ON;
      LED3_ON;
      
      SCM_state = SCM_RUN;
      HAL_TIM_Base_Start_IT(&htimx);
    }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT11_GPIO_PIN);
  }
  else if(GPIO_Pin==INPUT12_GPIO_PIN)//关
  {
    HAL_Delay(20);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(INPUT12_GPIO,INPUT12_GPIO_PIN)==INPUT12_DOWN_LEVEL)
    {
      
      LED1_OFF;
      LED2_OFF;
      LED3_OFF;
      
      SCM_state = SCM_STOP;
      HAL_TIM_Base_Stop(&htimx);
   
    }
    __HAL_GPIO_EXTI_CLEAR_IT(INPUT12_GPIO_PIN);
  }
  
  Save_Device_Data(Rx_buf);
  
  
}





/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
