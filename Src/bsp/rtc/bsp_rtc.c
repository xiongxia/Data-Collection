/**
  ******************************************************************************
  * 文件名程: bsp_rtc.c 
  * 作    者: 
  * 版    本: V1.0
  * 编写日期: 2017-03-30
  * 功    能: RTC实时时钟
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "rtc/bsp_rtc.h"
#include "usart/bsp_debug_usart.h"


/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
//时钟
RTC_HandleTypeDef hrtc;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 把ASCII码转换为数字,把字符串转成十六进制
  * 输入参数: value 用户在超级终端中输入的数值
  * 返 回 值: 输入字符的ASCII码对应的数值
  * 说    明：本函数专用于RTC获取时间
  */
int StringToArray(char *str, unsigned char *out)
{

    char *p = str;

    char high = 0, low = 0;

    int tmplen = strlen(p);
    int cnt = 0;

    tmplen = strlen(p);

    if(tmplen % 2 != 0) return -1;

    while(cnt < tmplen / 2)

    {

        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;

        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;

        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));

        p++;

        cnt ++;

    }


    return tmplen / 2;

}
/**
  * 函数功能: 配置当前时间和日期
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void RTC_CalendarConfig(char *time)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;
  int outlen = 0;
  unsigned char out[10] = {0};
  
  outlen = StringToArray(time, out);
  Show_Data(out,7); 
  
  if(outlen != 7)
  {
    printf("RTC_CalendarConfig:配置时间错误,字段数量为%d，请求重发时间\n",outlen);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  
  /* 配置日期 */
  /* 设置日期：2018年11月28日 星期二*/
  if(out[0] < 0x18 || out[0] > 0x40)
  {
    printf("RTC_CalendarConfig:配置年份错误%02x，请求重发\n",out[0]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  
  if(out[1] > 0x12)
  {
    printf("RTC_CalendarConfig:配置月份错误%02x，请求重发\n",out[1]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  if(out[2] > 0x31)
  {
    printf("RTC_CalendarConfig:配置日期错误%02x，请求重发\n",out[2]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  if(out[3] > 0x07)
  {
    printf("RTC_CalendarConfig:配置星期错误%02x，请求重发\n",out[3]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  if(out[4] > 0x24)
  {
    printf("RTC_CalendarConfig:配置小时错误%02x，请求重发\n",out[4]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  if(out[5] > 0x60)
  {
    printf("RTC_CalendarConfig:配置分钟错误%02x，请求重发\n",out[5]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
  if(out[6] > 0x60)
  {
    printf("RTC_CalendarConfig:配置秒数错误%02x，请求重发\n",out[6]);
    HAL_UART_Transmit(&husart_debug,"+",1,1000);
    return;
  }
    /* 配置时间 */
  /* 时钟时间：10:15:46 */
  sTime.Hours = out[4];
  sTime.Minutes = out[5];
  sTime.Seconds = out[6];
  
  
  DateToUpdate.Year = out[0];  
  DateToUpdate.Month = out[1];
  DateToUpdate.Date = out[2];
  DateToUpdate.WeekDay = out[3];
  
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
  HAL_RTC_SetDate(&hrtc,&DateToUpdate,RTC_FORMAT_BCD);
  
  

  /* 写入一个数值：0x32F1到RTC备份数据寄存器1 */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F1);
}

/**
  * 函数功能: RTC实时时钟初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void MX_RTC_Init(void)
{
  /* 初始化RTC实时时钟并设置时间和日期 */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;  //24小时制
  hrtc.Init.AsynchPrediv = 127;              //异步除数
  hrtc.Init.SynchPrediv = 255;               //同步除数
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;     //不使能RTC_ALARM输出的标志  
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;  //RTC_ALARM输出极性为高
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;     //RTC_ALARM输出类型为开漏输出
  HAL_RTC_Init(&hrtc);

  //char *strCom = "18121704121700";
  /* 检测数据是否保存在RTC备份寄存器1：如果已经保存就不需要运行日期和时间设置 */
  /* 读取备份寄存器1数据 */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F1)
  {
    /* 配置RTC万年历：时间和日期 */
   // RTC_CalendarConfig(strCom);
  }
  
  else
  {
    /* 检查上电复位标志位是否为：SET */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      printf("发生上电复位！！！\n");
    }
    /* 检测引脚复位标志位是否为：SET */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      printf("发生外部引脚复位！！！\n");
    }
    /* 清楚复位源标志位 */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
}

/**
  * 函数功能: RTC实时时钟初始化
  * 输入参数: hrtc：RTC外设句柄指针
  * 返 回 值: 无
  * 说    明: 该函数供HAL库内部函数调用
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  if(hrtc->Instance==RTC)
  {
    /* To change the source clock of the RTC feature (LSE, LSI), You have to:
       - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
       - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
         configure the RTC clock source (to be done once after reset).
       - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
         __HAL_RCC_BACKUPRESET_RELEASE().
       - Configure the needed RTc clock source */

    HAL_PWR_EnableBkUpAccess();
    
    /* 使能备份时钟：备份寄存器 */
  //  __HAL_RCC_BKP_CLK_ENABLE();
    
    /* 配置外部低速时钟为RTC时钟源 */
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;             //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; //外设为RTC
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; //RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    
    /* 使能RTC外设时钟 */
    __HAL_RCC_RTC_ENABLE();
  }
  
}

/**
  * 函数功能: RTC实时时钟反初始化
  * 输入参数: hrtc：RTC外设句柄指针
  * 返 回 值: 无
  * 说    明: 该函数供HAL库内部函数调用
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{

  if(hrtc->Instance==RTC)
  {
    /* 禁用RTC时钟 */
    __HAL_RCC_RTC_DISABLE();

    /* 禁用PWR时钟和读取备份域 */
    HAL_PWR_DisableBkUpAccess();
    __HAL_RCC_PWR_CLK_DISABLE();
    
    /* 禁用备份时钟：备份寄存器 */
    //__HAL_RCC_BKP_CLK_DISABLE();
  }
} 



