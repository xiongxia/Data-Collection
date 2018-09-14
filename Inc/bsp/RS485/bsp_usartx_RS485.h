#ifndef __BSP_USARTX_H__
#define __BSP_USARTX_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define RS485_USARTx                                 USART3
#define RS485_USARTx_BAUDRATE                        9600
#define RS485_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART3_CLK_ENABLE()
#define RS485_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART3_CLK_DISABLE()

#define RS485_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_USARTx_Tx_GPIO_PIN                     GPIO_PIN_10
#define RS485_USARTx_Tx_GPIO                         GPIOB
#define RS485_USARTx_Rx_GPIO_PIN                     GPIO_PIN_11   
#define RS485_USARTx_Rx_GPIO                         GPIOB

#define RS485_USARTx_CTRL_GPIO_ClK_ENABLE()          __HAL_RCC_GPIOH_CLK_ENABLE()
#define RS485_USARTx_CTRL_PIN                        GPIO_PIN_8
#define RS485_USARTx_CTRL_GPIO                       GPIOH

#define RX_MODE()                               HAL_GPIO_WritePin(RS485_USARTx_CTRL_GPIO,RS485_USARTx_CTRL_PIN,GPIO_PIN_RESET)
#define TX_MODE()                               HAL_GPIO_WritePin(RS485_USARTx_CTRL_GPIO,RS485_USARTx_CTRL_PIN,GPIO_PIN_SET)

#define RS485_USARTx_AFx                             GPIO_AF7_USART3

#define RS485_USARTx_IRQHANDLER                      USART3_IRQHandler
#define RS485_USARTx_IRQn                            USART3_IRQn



/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husartx_rs485;

/* 函数声明 ------------------------------------------------------------------*/
void RS485_USARTx_Init(void);
void RS485_Send_Data(uint8_t *buf,uint8_t len);
uint16_t RS485_Receive_Data();


#endif  /* __BSP_USARTX_RS485_H__ */


