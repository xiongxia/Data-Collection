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
#include "iwdg/bsp_iwdg.h"

    
/* 私有类型定义 --------------------------------------------------------------*/

__IO uint16_t minute_timer_count = 0;//定时器计数 分
__IO uint16_t second_timer_count = 0;//定时器计数 秒

__IO uint8_t error = 0;//程序因为串口或者长时间未接收到安卓通信消息标志位，单片机自动重启但是不给安装发送消息

uint8_t recv_flag_save = 0;//保存接收安卓数据个数，用来判断是否接收完成
__IO uint8_t keyTimeFlag = 0;//控制防止移液泵控制抖动

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
uint8_t debug_test = 1;
uint8_t aRxBuffer_485 = 0;
int8_t frist = -1;
__IO uint8_t RevDevicesData = 0;
__IO uint8_t RevCommand = 0;
__IO uint8_t com_flag = 0;
__IO uint8_t Rx_Count = 0;
uint8_t aRxBuffer_Android = 0;
uint8_t recv_flag_num = 0;
__IO uint16_t delayContorl = 0;
__IO uint8_t DownloadFirmwareFlag = 0; 
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
    uint8_t info[5];
    RTC_TimeTypeDef stimestructureget;
    RTC_DateTypeDef sdatestructureget;
    
    // 复位所有外设，初始化Flash接口和系统滴答定时器 
    HAL_Init();
    //配置系统时钟 
    SystemClock_Config();
    MX_SPIFlash_Init();
    MX_USARTx_Init();
    //初始化RTC实时时钟 
    MX_RTC_Init();
    
    RS485_USARTx_Init();
    // 板载LED初始化 
    LED_GPIO_Init();
    //板子输出初始化 
    OUTPUT_GPIO_Init();
    //输入初始化
    INPUT_GPIO_Init();
    //基本定时器初始化:1ms中断一次 
    BASIC_TIMx_Init();
    MX_DEBUG_USART_Init();
    printf("远创新智——测试6.11\n");
      
    //使能接收，进入中断回调函数 
    HAL_UART_Receive_IT(&husart_debug,&aRxBuffer_Android,1);
    HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer_485,1);

    //接收485数据
    RX_MODE();
    //显示开始时间
    RTC_CalendarShow();
    //发送版本号
    sprintf(info,"|46");
    HAL_UART_Transmit(&husart_debug,info,strlen((char *)info),1000); 
    //关闭灯、移液泵
   // Close_YiYe_pupm();
    //Close_Light();
   // Close_Beep();
  
    //获取flash配置
    Get_Device_Data(Android_Rx_buf);

    //重启默认运行
    SCM_state = SCM_RUN;
    //Open_Light();
    LED1_ON;
    
    //在中断模式下启动定时器 
    HAL_TIM_Base_Start_IT(&htimx);
     /* IWDG 5s 超时溢出 */ 
    MX_IWDG_Init(IWDG_PRESCALER_64,3125);
     /* 启动独立看门狗 */
    __HAL_IWDG_START(&hiwdg);
   
    
    //进入主程序循环
    while (1)
    {
        HAL_IWDG_Refresh(&hiwdg);
        //开始采集
        if(Sample_flag)
        {
           //采集数据喂狗
           HAL_IWDG_Refresh(&hiwdg);
           Sample_flag = 0;
           LED2_TOGGLE;
           Sample_RS485();
           delay_ms2(100);
         }
        //接收到安卓的配置信息
        if(RevDevicesData)
        {
           //清除标志位
            UpData_flag = 0;
            RevDevicesData = 0;
            printf("安卓:%s\n",Android_Rx_buf);
            //推迟frist控制时间
            frist = -1;
            //计时器清零
            second_timer_count = 0;
            minute_timer_count = 0;;
            time_num = 0;
            //保存数据
            Save_Data();
            FLAG = 0;
        }
        //接收到安卓的命令信息
        if(RevCommand)
        {
            RevCommand= 0;
            printf("接收命名信息:%s",Android_Rx_buf);
            Command_Data();  
            FLAG = 0;
        }
        //接收到重启指令
        if(Reboot_flag)
        {
            //RTC_CalendarShow();
            FLAG = 1;
            Reboot_flag = 0;
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
        }
        //上传数据
        if(UpData_flag)
        {
            UpData_flag = 0;
            //RTC_CalendarShow();
            FLAG = 1;
            LED3_TOGGLE;
            //RTC_CalendarShow();
            Get_Average();
            UpData();
            //test
           // delayContorlFlag = 1;
            Control();
            //Detection();
            FLAG = 0;
        }
        if(Control_flag && frist == 1 && Android_Rx_Count <= 5)
        {
            FLAG = 1;
            Control_flag = 0;
            //第一分钟不控制
            printf("开始控制逻辑\n");
            LED3_TOGGLE;
            Control();
            FLAG = 0;
            delay_ms2(100);
        }
        //更新时间
        if(RTC_Config_flag)
        {
           RTC_Config_flag = 0;
           RTC_CalendarConfig(Android_Rx_buf);
           RTC_CalendarShow();
           FLAG = 0;
        }
        //停止后重启，恢复状态
        if(Save_flag)
        {
            //RTC_CalendarShow();
            FLAG = 1;
            Save_flag = 0;
            if(SCM_state == SCM_RUN)
            {
                Open_Light();
                for(i = 0; i < 5; i++)
                {
                    if (delay[i].control != 0 && delay[i].state == 1)
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
                Close_Delay('1');
                Close_Delay('2');
                Close_Delay('3');
                Close_Delay('4');
                Close_Delay('5');
                Close_YiYe_pupm();
                Close_Light();
                Close_Beep();
            }
            FLAG = 0;
        }
        //控制移液泵
        if(YiYe_pump_control_flag)
        {
            YiYe_pump_control_flag = 0;
            if(YiYe_pump_flag)
            {
                Open_YiYe_pupm();
            }
            else
            {
                Close_YiYe_pupm();
            }
            
        }
        //解除告警
        if(Error_flag)
        {
            Error_flag = 0;
            Close_Error(Android_Rx_buf[0]);
            FLAG = 0;
        }
        //清除flash内存
        if(clear_flag)
        {
            clear_flag = 0;
            Clear_FLASH_Data();
            
        }
        //开启手动控制
        if(ManualContorlFlag)
        {
            ManualContorlFlag = 0;
            
            if(Android_Rx_buf[0] == '1')
            {
                openManualContorl();
                HAL_UART_Transmit(&husart_debug,"(1",2,1000);
            }
            else
            {
                closeManualContorl();
                HAL_UART_Transmit(&husart_debug,"(0",2,1000);
            }
        }
        //下载更新
        if(DownloadFirmwareFlag)
        {
            DownloadFirmwareFlag = 0;
            uint8_t sizeRead[5] = {0};
            Android_Rx_buf[4] = '\0';
            printf("CRC验证码:%c%c%c%c\n",Android_Rx_buf[0],Android_Rx_buf[1],Android_Rx_buf[2],Android_Rx_buf[3]);
            sprintf(ErrorInfo,"main:CRC code: %c%c%c%c\n",Android_Rx_buf[0],Android_Rx_buf[1],Android_Rx_buf[2],Android_Rx_buf[3]);
            Log();
            //Show_Data(Android_Rx_buf,strlen(Android_Rx_buf)); 
            SPI_FLASH_SectorErase(FLASH_CRC_CODE);
            SPI_FLASH_BufferWrite(Android_Rx_buf, FLASH_CRC_CODE, 4);
            SPI_FLASH_BufferRead(sizeRead, FLASH_CRC_CODE,4);
            //Show_Data(sizeRead,4); 
            printf("CRC验证码:%c%c%c%c\n",sizeRead[0],sizeRead[1],sizeRead[2],sizeRead[3]);
            //写入成功
            sizeRead[4] = '\0';
            if(sizeRead[0] == Android_Rx_buf[0] && sizeRead[1] == Android_Rx_buf[1] && sizeRead[2] == Android_Rx_buf[2] && sizeRead[3] == Android_Rx_buf[3])
            {
                Reboot_flag = 1;
            }
            else
            {
                sprintf(ErrorInfo,"main:CRC code write fail\n");
                Log();
                HAL_TIM_Base_Start_IT(&htimx);
            }
        }
        //延迟控制模式六
        if(delayContorl > 6000 * 10 && !delayContorlFlag)
        {
            printf("延迟控制结束\n");
            delayContorlFlag = 1;
        }
        //接收到安卓数据时候不进行采集等操作
        if(Android_Rx_Count <= 5)
        {
            //3s钟采集一次
            if(second_timer_count % 495 >= 490)
            {
                //有配置信息采集
                if(Sensor_Cfg_Mode)
                {
                    Sample_flag = 1;
                }   
                //按键消除抖动
                if(keyTimeFlag)
                {
                    keyTimeFlag = 0;
                }
            }
            if(second_timer_count >= 1007)
            {
                second_timer_count = 0; 
                //10秒发送一次心跳        
                sprintf(info,"@%d",SCM_state);
                HAL_UART_Transmit(&husart_debug,info,strlen((char *)info),1000); 
                printf("发送心跳\n");
                //检查是否接受安卓数据不完整
                if(recv_flag_save == Android_Rx_Count)
                {
                    recv_flag_num ++;
                    if(recv_flag_num == 2)
                    {
                      Android_Rx_Count = 0;
                      recv_flag_num = 0;
                    }
                }
            }
            if(Delay_Cfg_Mode && minute_timer_count % 914 >= 913)
            {
                Control_flag = 1;
            }
            //1分钟上传一次
            if(minute_timer_count >= 6011)
            {
                // LED3_TOGGLE;
                minute_timer_count = 0;
                //有配置信息采集
                if(Sensor_Cfg_Mode)
                {
                    UpData_flag = 1;
                    if(frist <= 0)
                    {
                        frist ++;
                    }
                }
             }
        }//if
        for (i = 0; i < 5; i++)
        {
            //if(delay[i].devices != 0 && delay[i].control >= 3 && delay[i].state == 1)
            //时间控制  3 4 6 模式下
            if(delay[i].control >= 3 && (time_Control_flag || sys_time_Control_flag) && delay[i].control != 5 && !delay[i].error)
            {
                 //获取当前时间 
                HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
                sprintf(start_time,"%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);   
                HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
                if(strcmp(delay[i].start_time,start_time) == 0)
                {
                    printf("计时开始，打开泵%d\n",i + 1);
                    //时间到
                    strcpy(delay[i].start_time,"00:00");
                    Open_Delay(delay[i].port);
                }
                else if(delay[i].counter <= 10)               
                {
                    Close_Delay(delay[i].port);
                    //确认关闭
                    if(delay[i].state == 0)
                    {
                        //计数结束
                        printf("计时结束，关闭泵%d\n",i+1);
                        if(delay[i].control == 3)
                        {
                            delay[i].counter = delay[i].save_counter;
                            Chang_Start_time(i);
                         }
                         else if(delay[i].control == 6)
                         {
                             printf("继电器关闭，模式6控制方式，还需要%d\n",time_num);
                         }
                         else if(delay[i].control == 4)
                         {
                              delay[i].counter = 1000;
                         }
                     }
                     else
                     {
                          delay[i].counter = 1000;
                     }
                     Control();
                }
                else 
                {
                }
            }
            if(time_num >= 6000 * 35)
            {
                if(com_flag)
                {
                  com_flag = 0;
                }
            else
            {
                printf("长时间未收到安卓通信消息，自动重启\n");
                Reboot_flag = 1;
                sprintf(ErrorInfo,"main:Communication failure\n");
                Log();
                error = 1;
            }
               time_num = 0;
               printf("30分钟计时结束，重新开始模式6控制方式\n");
               index_time_control_flag = 0;
            }
        }
           
        //数据太多清除缓存
       if(Rx_Count >= 200){
          Rx_Count = 0;
          FLAG = 0;
        }
        if(Android_Rx_Count >= 900) 
        {
           Android_Rx_Count = 0;
        }
        if(RS485_Rx_Count >= 49) 
        {
           RS485_Rx_Count = 0;
        }
        //告警延时
        if(warn_timer_count <= 100 && warn_timer_count != 0)
        {
           Close_Beep();
           Open_Light();
           warn_timer_count = 0;
        }
  }//while
}//mian

/**
  * 函数功能: 串口接收完成回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明:无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    //安卓端接收
    uint8_t error_num = 0;
    
    if(UartHandle->Instance == DEBUG_USARTx)
    {
        //单片机在运行其他控制程序，不保存安卓数据
        if(FLAG)
        {
           // printf("安卓接收但不保存数据:%c\n",aRxBuffer_Android);
            Rx_Count++;
            Android_Rx_Count = 0;
            //只接收通信消息
            if(aRxBuffer_Android == '{')
            {
                FLAG = 0;
                HAL_UART_Transmit(&husart_debug,"{",1,1000);
                com_flag = 1;      
                printf("安卓通信信息:接收完成\n");
            }
        }
        //正常接收安卓数据，并且缓存没有到达上限
        else if(Android_Rx_Count < 1000)
        {
            //printf("安卓接收保存数据:%c\n",aRxBuffer_Android);
            if(Android_Rx_Count != 0 || aRxBuffer_Android != 0x20)
            {
                Android_Rx_buf[Android_Rx_Count] = aRxBuffer_Android;
                Android_Rx_Count++;
            }
            recv_flag_save = Android_Rx_Count; 
            switch(aRxBuffer_Android)
            {
                case '#': 
                    FLAG = 1;
                    RevDevicesData = 1;
                    Android_Rx_buf[Android_Rx_Count - 1] = '\0';
                    printf("安卓配置信息:接收完成\n");
                    Android_Rx_Count = 0;
                    break;
                case '!': 
                    FLAG = 1;
                    RevCommand = 1;
                    Android_Rx_buf[Android_Rx_Count - 1] = '\0';
                    printf("安卓命令信息:接收完成\n");
                    Android_Rx_Count = 0;
                    break;
                //解除警告
                case '$':
                    if(Android_Rx_Count < 5)
                    {
                        FLAG = 1;
                        Android_Rx_buf[Android_Rx_Count - 1] = '\0';
                        Error_flag = 1;
                        printf("安卓解除信息:接收完成\n");
                        Android_Rx_Count = 0;
                    }
                    break;
                case ':' :
                    if(Android_Rx_Count >= 15 && Android_Rx_Count <= 17)
                    {
                        FLAG = 1;
                        Android_Rx_buf[Android_Rx_Count - 1] = '\0';
                        RTC_Config_flag = 1;
                        printf("安卓更新时间信息:接收完成\n");      
                        Android_Rx_Count = 0;
                     }
                     break;
                case '{':
                  //通信
                        HAL_UART_Transmit(&husart_debug,"{",1,1000);
                        com_flag = 1;
                        Android_Rx_buf[Android_Rx_Count - 1] = '\0';       
                        printf("安卓通信信息:接收完成\n");
                        Android_Rx_Count = 0;
                        break;
                case '&':
                   //更新系统
                    if(Android_Rx_Count < 10)
                    {
                        //接收程序
                        HAL_TIM_Base_Stop(&htimx);
                        FLAG = 1;
                        printf("系统准备重启，更新程序\n");
                        sprintf(ErrorInfo,"main:update system\n");
                        Log();
                        //Reboot_flag = 1;
                        DownloadFirmwareFlag = 1;
                     }
                     break;
                case '%':
                   //重启系统
                     if(Android_Rx_Count < 5)
                     {
                         FLAG = 1;
                         printf("系统准备重启\n");
                         error = 1;
                         sprintf(ErrorInfo,"main:android reboot\n");
                         Log();
                         Reboot_flag = 1;
                         HAL_TIM_Base_Stop(&htimx);
                     }
                     break;
                case '@':
                     if(Android_Rx_Count < 5)
                     {
                         clear_flag = 1;
                         printf("安卓清除单片机FLASH接收完成\n");
                         Android_Rx_Count = 0;
                     }
                     break;
                case '(':
                     if(Android_Rx_Count < 5)
                     {
                         ManualContorlFlag = 1;
                         printf("手动控制接收完成\n");
                         Android_Rx_Count = 0;
                     }
                     break;
                default:
                    break;
            }
        }
        else
        {
            printf("输出数据溢出\n");
            Android_Rx_Count = 0;
        }
        HAL_UART_Receive_IT(&husart_debug,&aRxBuffer_Android,1);
  /*
     do{
          if(UartHandle->RxState == HAL_UART_STATE_READY && !FLAG){
            HAL_UART_Receive_IT(&husart_debug,&aRxBuffer_Android,1);
            break;
          }
          error_num++;
          if(error_num >=10){
            error_num = 0;
            printf("232串口堵塞，自动重启\n");
            //HAL_UART_Receive(&husart_debug,(uint8_t *)aRxBuffer_Android,500,1000);
		
            Reboot_flag = 1;
            error = 1;
            break;
          }
     }while(1);*/
    
    }
    //485接收
    else if(UartHandle->Instance == RS485_USARTx)
    {
        //printf("\n485:%02x\n",aRxBuffer_485);
        RS485_Rx_buf[RS485_Rx_Count] = aRxBuffer_485;
        RS485_Rx_Count ++;
		/*
    if(aRxBuffer_485 == saveRS485Adder && RS485_Rx_Count == 1){
      printf("485\n");
      RS485_Rx_Count = 1;
    }
    else{
      RS485_Rx_Count = 0;
    }*/
    do{
          if(UartHandle->RxState == HAL_UART_STATE_READY){
            HAL_UART_Receive_IT(&husartx_rs485,&aRxBuffer_485,1);
            break;
          }
          error_num++;
          if(error_num >=10)
          {
            error_num = 0;
            printf("485串口堵塞，自动重启\n");
            sprintf(ErrorInfo,"main:485 reboot\n");
            Log();
            Reboot_flag = 1;
            error = 1;
            break;
          }
     }while(1);
    }
}
/**
  * 函数功能: 非阻塞模式下定时器的回调函数
  * 输入参数: htim:定时器句柄
  * 返 回 值: 无
  * 说    明: 100ms 中断一次
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
    //采集定时
    second_timer_count++;
    minute_timer_count++;
    time_num++;
    
    //延时控制计时
    if(!delayContorlFlag)
        delayContorl ++;
    //告警定时
    if(warn_timer_count >= 10)
    {
        warn_timer_count --;
    }
    for(int i=0;i<5;i++)
    {
        //继电器控制计时，排除指标，时间辅助控制方式，手动控制的情况
        if(delay[i].control >= 3 && delay[i].control != 5 && (time_Control_flag || sys_time_Control_flag))
        {
            //如果手动控制，取消本次被打断的控制
            if(delay[i].error)
            {
                delay[i].counter = 1000;
                continue;
            }
            //继电器打开才开始计时
            if( delay[i].state == 1)
                delay[i].counter --;
            //继电器根据时间控制，但是被打断了，现在已经关闭了，所以这次不再控制了,10s后再次进行控制流程
            else if(delay[i].counter != delay[i].save_counter)
                delay[i].counter = 1000;
            else
            {
                //还没开始计时，控制
            }
        }
    }
}

/**
  * 函数功能: 按键外部中断服务函数
  * 输入参数: GPIO_Pin:中断引脚
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    printf("检测到输入电平  %x\n",GPIO_Pin);
 
    //开
    if(GPIO_Pin == INPUT11_GPIO_PIN)
    {
        LED1_ON;
        if(SCM_state != SCM_RUN)
        {
            SCM_state = SCM_RUN;
            Save_flag = 1;
        }
        HAL_TIM_Base_Start_IT(&htimx);
        __HAL_GPIO_EXTI_CLEAR_IT(INPUT11_GPIO_PIN);
    }
    //关
    else if(GPIO_Pin == INPUT12_GPIO_PIN)
    {
        LED1_OFF;
        if(SCM_state != SCM_STOP)
        {
            SCM_state = SCM_STOP;
            Save_flag = 1;
        }
        HAL_TIM_Base_Stop(&htimx);
        __HAL_GPIO_EXTI_CLEAR_IT(INPUT12_GPIO_PIN);
    }
    //移液泵
    else if(GPIO_Pin == INPUT10_GPIO_PIN)
    {
        if(!keyTimeFlag)
        {
            YiYe_pump_control_flag = 1;
            YiYe_pump_flag = YiYe_pump_flag ^ 0xFE;
            keyTimeFlag = 1;
        }
        __HAL_GPIO_EXTI_CLEAR_IT(INPUT10_GPIO_PIN);
    }
    //急停
    else if(GPIO_Pin == INPUT9_GPIO_PIN)
    {
        LED1_OFF;
        if(SCM_state != SCM_STOP)
        {
            SCM_state = SCM_STOP;
            Save_flag = 1;
        }
        HAL_TIM_Base_Stop(&htimx);
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
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟:168MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟: 168MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1时钟:42MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2时钟:84MHz
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


