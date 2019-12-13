
#ifndef  __SPI_DMA_DEF_H
#define __SPI_DMA_DEF_H

#include <stdint.h>
#include "GlobalDef.h"

/* Exported typedef ----------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))


/* Exported define -----------------------------------------------------------*/
/* Uncomment the line below if you will use the SPI peripheral as a Master */
 #define SPI_MASTER 
/* Uncomment the line below if you will use the SPI peripheral as a Slave */


/* USER_TIMEOUT value for waiting loops. This timeout is just guarantee that the
   application will not remain stuck if the USART communication is corrupted. 
   You may modify this timeout value depending on CPU frequency and application
   conditions (interrupts routines, number of data to transfer, baudrate, CPU
   frequency...). */ 
#define USER_TIMEOUT                    ((uint32_t)0x64) /* Waiting 1s */


#include "board_version.h"
/* SPIx Communication boards Interface */

  #define FL_SPIx                                                 SPI1
  #define FL_SPIx_CLK                                     RCC_APB2Periph_SPI1
  #define FL_SPIx_CLK_INIT                         RCC_APB2PeriphClockCmd
  #define FL_SPIx_IRQn                                   SPI1_IRQn
  #define FL_SPIx_IRQHANDLER                SPI1_IRQHandler

  #define FL_SPIx_SCK_PIN                               GPIO_Pin_5
  #define FL_SPIx_SCK_GPIO_PORT            GPIOA
  #define FL_SPIx_SCK_GPIO_CLK                RCC_AHB1Periph_GPIOA
  #define FL_SPIx_SCK_SOURCE                    GPIO_PinSource5
  #define FL_SPIx_SCK_AF                                 GPIO_AF_SPI1

  #define FL_SPIx_MISO_PIN                            GPIO_Pin_6
  #define FL_SPIx_MISO_GPIO_PORT         GPIOA
  #define FL_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
  #define FL_SPIx_MISO_SOURCE                 GPIO_PinSource6
  #define FL_SPIx_MISO_AF                              GPIO_AF_SPI1

  #define FL_SPIx_MOSI_PIN                            GPIO_Pin_7
  #define FL_SPIx_MOSI_GPIO_PORT         GPIOA
  #define FL_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
  #define FL_SPIx_MOSI_SOURCE                 GPIO_PinSource7
  #define FL_SPIx_MOSI_AF                              GPIO_AF_SPI1

  #define FL_SPIx_DMA                                               DMA2
  
  #define FL_SPIx_DMA_CLK                                   RCC_AHB1Periph_DMA2
  #define FL_SPIx_TX_DMA_CHANNEL              DMA_Channel_3
  #define FL_SPIx_TX_DMA_STREAM                DMA2_Stream3
  #define FL_SPIx_TX_DMA_FLAG_TCIF          DMA_FLAG_TCIF3   // TCIFx = Streamx, x: 0 - 3
  
  #define FL_SPIx_RX_DMA_CHANNEL             DMA_Channel_3
  #define FL_SPIx_RX_DMA_STREAM                DMA2_Stream2
  #define FL_SPIx_RX_DMA_FLAG_TCIF          DMA_FLAG_TCIF2    // TCIFx = Streamx, x: 0 - 3

#define FLASH_CS_Pin    GPIO_Pin_4
#define FLASH_CS_PORT   GPIOA


#define BUFFERSIZE                      4096


#define  FL_SPI_DMA_EN     0 // SPI ÊÇ·ñÊ¹ÄÜDMA 

void spi_dma_init(void);
uint8_t spi_dma_send(const uint8_t * tx_buf, uint32_t tx_size, uint8_t * rx_buf,  uint32_t rx_size);
void spi_dma_demo(void);

#endif

