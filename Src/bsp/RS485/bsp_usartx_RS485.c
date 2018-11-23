/**
  ******************************************************************************
  * 文件名程: bsp_usartx_RS485.c 
  * 作    者: 
  * 版    本: V1.0
  * 编写日期: 2017-03-30
  * 功    能: 板载串口底层驱动程序
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "RS485/bsp_usartx_RS485.h"
#include "variable.h"
#include "function.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
UART_HandleTypeDef husartx_rs485;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: NVIC配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void MX_NVIC_USART3_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RS485_USARTx_IRQn,1,0);
  HAL_NVIC_EnableIRQ(RS485_USARTx_IRQn);
}


/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void RS485_USARTx_Init(void)
{ 
  /* 使能串口功能引脚GPIO时钟 */
  RS485_USARTx_GPIO_ClK_ENABLE();
  
  husartx_rs485.Instance = RS485_USARTx;
  husartx_rs485.Init.BaudRate = RS485_USARTx_BAUDRATE;
  husartx_rs485.Init.WordLength = UART_WORDLENGTH_8B;
  husartx_rs485.Init.StopBits = UART_STOPBITS_1;
  husartx_rs485.Init.Parity = UART_PARITY_NONE;
  husartx_rs485.Init.Mode = UART_MODE_TX_RX;
  husartx_rs485.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx_rs485.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx_rs485);
  
  /* 配置串口中断并使能，需要放在HAL_UART_Init函数后执行修改才有效 */
  MX_NVIC_USART3_Init();
}
/**
  * 函数功能: RS485发送len个字节.
  * 输入参数: buf:发送区首地址，len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
  * 返 回 值: 无
  * 说    明：无
  */
void RS485_Send_Data(uint8_t *buf,uint8_t len){
    //int i = 0;
    /* 进入发送模式 */
    TX_MODE();
    /* 发送数据,轮询直到发送数据完毕 */
    if(HAL_UART_Transmit(&husartx_rs485,buf,len,0xFFFF)==HAL_OK)
    {
      while(__HAL_UART_GET_FLAG(&husartx_rs485,UART_FLAG_TC)!=1);
      /* 进入接收模式 */
        RX_MODE();
        Show_Data(RS485_Rx_buf,20);  
      //  Sample_flag = 1;
      /* 使用调试串口打印调试信息到串口调试助手 */
     // Show_Data(buf,8);  
        printf("\nRS485发送数据成功:"); 
        Show_Data(buf,len); 
    }
}
/**
  * 函数功能: RS485查询接收到的数据
  * 输入参数: buf:接收缓存首地址，len:读到的数据长度
  * 返 回 值: 无
  * 说    明：
  */
void RS485_Receive_Data(uint8_t *len){
  
	*len=0;				//默认为0
	
    //if(Sample_flag == 1){
   // while(1){
        printf("数据采集中\n");
        HAL_Delay(500);//20ms接收
        //if(RS485_Rx_Count == RS485_Rx_Count_Old){
        printf("传感器接收完成\n");
        Show_Data(RS485_Rx_buf,20);  
        printf("接收长度：%d\n",RS485_Rx_Count);
        *len = RS485_Rx_Count;
        if(*len == 0){
            printf("数据采集失败\n");
        }
        RS485_Rx_Count_Old = RS485_Rx_Count;
        RS485_Rx_Count = 0; 
        //Sample_flag = 0;    
 //}//while
}



