/**
  ******************************************************************************
  * ????: main.c 
  * ?    ?: ?????????
  * ?    ?: V1.0
  * ????: 2017-03-30
  * ?    ?: ????????????:????USART1
  ******************************************************************************
  * ??:
  * ???????stm32???YS-F4Pro???
  * 
  * ??:
  * ??:http://www.ing10bbs.com
  * ??????????????,?????
  ******************************************************************************
  */
/* ????? ----------------------------------------------------------------*/  
#include "usart/bsp_debug_usart.h"
#include "RS485/bsp_usartx_RS485.h"

/* ?????? --------------------------------------------------------------*/
/* ????? ----------------------------------------------------------------*/
/* ???? ------------------------------------------------------------------*/
UART_HandleTypeDef husart_debug;
UART_HandleTypeDef husartx;

/* ???? ------------------------------------------------------------------*/
/* ?????? --------------------------------------------------------------*/
/* ??? --------------------------------------------------------------------*/


/**
  * ????: NVIC??
  * ????: ?
  * ? ? ?: ?
  * ?    ?: ?
  */
static void MX_NVIC_USART1_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DEBUG_USART_IRQn ,0, 2);
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQn);
}

/**
  * ????: NVIC??
  * ????: ?
  * ? ? ?: ?
  * ?    ?: ?
  */
static void MX_NVIC_USART5_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USARTx_IRQn,3, 0);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
}
/**
  * ????: ?????????
  * ????: huart:????????
  * ? ? ?: ?
  * ?    ?: ????HAL?????
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  if(huart->Instance==DEBUG_USARTx)
  {
    /* ???????? */
    DEBUG_USART_RCC_CLK_ENABLE();
  
    /* ??????GPIO?? */
    GPIO_InitStruct.Pin = DEBUG_USARTx_Tx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = DEBUG_USARTx_AFx;
    HAL_GPIO_Init(DEBUG_USARTx_Tx_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = DEBUG_USARTx_Rx_GPIO_PIN;  
    HAL_GPIO_Init(DEBUG_USARTx_Tx_GPIO, &GPIO_InitStruct);       
  }

   else if(huart->Instance==RS485_USARTx)
  {
    /* ???????? */
    RS485_USART_RCC_CLK_ENABLE();
    RS485_USARTx_CTRL_GPIO_ClK_ENABLE();
    
    /* ??????GPIO?? */
    GPIO_InitStruct.Pin = RS485_USARTx_Tx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = RS485_USARTx_AFx;
    HAL_GPIO_Init(RS485_USARTx_Tx_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = RS485_USARTx_Rx_GPIO_PIN;
    HAL_GPIO_Init(RS485_USARTx_Rx_GPIO, &GPIO_InitStruct);
    
    
    GPIO_InitStruct.Pin = RS485_USARTx_CTRL_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(RS485_USARTx_CTRL_GPIO, &GPIO_InitStruct);
  }
  else if(huart->Instance == USARTx)
  {
    /* ????????GPIO?? */
    USARTx_GPIO_ClK_ENABLE();
  
    /* ??????GPIO?? */
    GPIO_InitStruct.Pin = USARTx_Tx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USARTx_AFx;
    HAL_GPIO_Init(USARTx_Tx_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = USARTx_Rx_GPIO_PIN;
    HAL_GPIO_Init(USARTx_Rx_GPIO, &GPIO_InitStruct);
  }
}

/**
  * ????: ??????????
  * ????: huart:????????
  * ? ? ?: ?
  * ?    ?: ????HAL?????
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==DEBUG_USARTx)
  {
    /* ???????? */
    DEBUG_USART_RCC_CLK_DISABLE();
  
    /* ??????GPIO?? */
    HAL_GPIO_DeInit(DEBUG_USARTx_Tx_GPIO, DEBUG_USARTx_Tx_GPIO_PIN);
    HAL_GPIO_DeInit(DEBUG_USARTx_Rx_GPIO, DEBUG_USARTx_Rx_GPIO_PIN);
    
    /* ?????? */
    HAL_NVIC_DisableIRQ(DEBUG_USART_IRQn);
  }
  else  if(huart->Instance==RS485_USARTx)
  {
    /* ???????? */
    USART_RCC_CLK_DISABLE();
  
    /* ??????GPIO?? */
    HAL_GPIO_DeInit(RS485_USARTx_Tx_GPIO, RS485_USARTx_Tx_GPIO_PIN);
    HAL_GPIO_DeInit(RS485_USARTx_Rx_GPIO, RS485_USARTx_Rx_GPIO_PIN);
    
    /* ?????? */
    HAL_NVIC_DisableIRQ(RS485_USARTx_IRQn);
  }
  else if(huart->Instance==USARTx)
  {
    /* ???????? */
    USART_RCC_CLK_DISABLE();
  
    /* ??????GPIO?? */
    HAL_GPIO_DeInit(USARTx_Tx_GPIO, USARTx_Tx_GPIO_PIN);
    HAL_GPIO_DeInit(USARTx_Rx_GPIO, USARTx_Rx_GPIO_PIN);
    
    /* ?????? */
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
  }
}
/**
  * ????: ??????.
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
void MX_USARTx_Init(void)
{
  /* ???????? */
  USART_RCC_CLK_ENABLE();
  
  husartx.Instance = USARTx;
  husartx.Init.BaudRate = USARTx_BAUDRATE;
  husartx.Init.WordLength = UART_WORDLENGTH_8B;
  husartx.Init.StopBits = UART_STOPBITS_1;
  husartx.Init.Parity = UART_PARITY_NONE;
  husartx.Init.Mode = UART_MODE_TX_RX;
  husartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx);
  
  /* ?????????,????HAL_UART_Init?????????? */
  MX_NVIC_USART5_Init();
}

/**
  * ????: ??????.
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
void MX_DEBUG_USART_Init(void)
{
  /* ????????GPIO?? */
  DEBUG_USARTx_GPIO_ClK_ENABLE();
  
  husart_debug.Instance = DEBUG_USARTx;
  husart_debug.Init.BaudRate = DEBUG_USARTx_BAUDRATE;
  husart_debug.Init.WordLength = UART_WORDLENGTH_8B;
  husart_debug.Init.StopBits = UART_STOPBITS_1;
  husart_debug.Init.Parity = UART_PARITY_NONE;
  husart_debug.Init.Mode = UART_MODE_TX_RX;
  husart_debug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husart_debug.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husart_debug);
   /* ?????????,????HAL_UART_Init?????????? */
  MX_NVIC_USART1_Init();
}

/**
  * ????: ???c???printf?USARTx
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&husartx, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * ????: ???c???getchar,scanf?DEBUG_USARTx
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&husartx,&ch, 1, 0xffff);
  return ch;
}

/******************* (C) COPYRIGHT 2015-2020 ????????? *****END OF FILE****/
