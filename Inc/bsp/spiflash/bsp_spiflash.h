#ifndef __BSP_SPIFLASH_H__
#define __BSP_SPIFLASH_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
//#define  SPI_FLASH_ID                       0xEF3015     //W25X16   2MB
//#define  SPI_FLASH_ID                       0xEF4015	   //W25Q16   4MB
//#define  SPI_FLASH_ID                       0XEF4017     //W25Q64   8MB
#define  SPI_FLASH_ID                       0XEF4018     //W25Q128  16MB YS-F4Pro开发默认使用

#define FLASH_SPIx                                 SPI2
#define FLASH_SPIx_RCC_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define FLASH_SPIx_RCC_CLK_DISABLE()               __HAL_RCC_SPI2_CLK_DISABLE()

#define FLASH_SPI_GPIO_ClK_ENABLE()                {__HAL_RCC_GPIOI_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();} 
#define FLASH_SPI_GPIO_PORT                        GPIOI
#define FLASH_SPI_SCK_PIN                          GPIO_PIN_1

#define FLASH_SPI_MOSI_PIN                         GPIO_PIN_3
    
#define FLASH_SPI_CS_PIN                           GPIO_PIN_0

#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(FLASH_SPI_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)

#define FLASH_SPI_MISO_PORT                        GPIOC
#define FLASH_SPI_MISO_PIN                         GPIO_PIN_2

#define  FLASH_WriteAddress     0x80FFF00
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

/* 扩展变量 ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspiflash;


/* 函数声明 ------------------------------------------------------------------*/

void MX_SPIFlash_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);
uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif  /* __BSP_SPIFLASH_H__ */

