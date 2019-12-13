
#include "spi_dma.h"
#include "os_global.h"


 uint8_t dma_buf[BUFFERSIZE * 2];
 uint8_t tmp_buf[BUFFERSIZE];

#define SPI_DMA_RETRY_PROCESS(delay, retry, result)                               \
    { void (*__delay_temp)(void) = (void (*)(void))delay;                        \
    if (retry == 0) {result = 1;break;}                         \
    else {if (__delay_temp) {__delay_temp();} retry --;}}

    



 SPI_InitTypeDef  SPI_InitStructure;
 
static void SPI_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
 
  
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  FL_SPIx_CLK_INIT(FL_SPIx_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(FL_SPIx_SCK_GPIO_CLK | FL_SPIx_MISO_GPIO_CLK | FL_SPIx_MOSI_GPIO_CLK, ENABLE);
  
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(FL_SPIx_DMA_CLK, ENABLE);

   while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}
  while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}

  
  /* SPI GPIO Configuration --------------------------------------------------*/
  /* GPIO Deinitialisation */
  GPIO_DeInit(FL_SPIx_SCK_GPIO_PORT);
  GPIO_DeInit(FL_SPIx_MISO_GPIO_PORT);
  GPIO_DeInit(FL_SPIx_MOSI_GPIO_PORT);
  
  /* Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(FL_SPIx_SCK_GPIO_PORT, FL_SPIx_SCK_SOURCE, FL_SPIx_SCK_AF);
  GPIO_PinAFConfig(FL_SPIx_MISO_GPIO_PORT, FL_SPIx_MISO_SOURCE, FL_SPIx_MISO_AF);    
  GPIO_PinAFConfig(FL_SPIx_MOSI_GPIO_PORT, FL_SPIx_MOSI_SOURCE, FL_SPIx_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = FL_SPIx_SCK_PIN;
  GPIO_Init(FL_SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);
  
  /* SPI  MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  FL_SPIx_MISO_PIN;
  GPIO_Init(FL_SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);  

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  FL_SPIx_MOSI_PIN;
  GPIO_Init(FL_SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);
  

        GPIO_InitStructure.GPIO_Pin = FLASH_CS_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL; 
        
        GPIO_Init(FLASH_CS_PORT, &GPIO_InitStructure);
       IO_H(FLASH_CS_PORT, FLASH_CS_Pin);
        
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(FL_SPIx);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(FL_SPIx, &SPI_InitStructure);
  
  /* DMA configuration -------------------------------------------------------*/
  /* Deinitialize DMA Streams */
  DMA_DeInit(FL_SPIx_TX_DMA_STREAM);
  DMA_DeInit(FL_SPIx_RX_DMA_STREAM);

  while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}
 while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}
  
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(FL_SPIx->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = FL_SPIx_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)dma_buf;
  DMA_Init(FL_SPIx_TX_DMA_STREAM, &DMA_InitStructure);
  
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_Channel = FL_SPIx_RX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)dma_buf ; 
  DMA_Init(FL_SPIx_RX_DMA_STREAM, &DMA_InitStructure);
  
}

/**
  * @brief  Configures the SPI Peripheral.
  * @param  None
  * @retval None
  */
void spi_dma_init(void)
{

     #if 0
     /* Enable DMA clock */
   RCC_AHB1PeriphClockCmd( FL_SPIx_DMA_CLK, ENABLE);
   FL_SPIx_CLK_INIT( FL_SPIx_CLK, ENABLE);
   
  while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}
  while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}
  
  /* DMA configuration -------------------------------------------------------*/
  /* Deinitialize DMA Streams */
  DMA_DeInit(FL_SPIx_TX_DMA_STREAM);
  DMA_DeInit(FL_SPIx_RX_DMA_STREAM);

  while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}
  while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}

  
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(FL_SPIx->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = FL_SPIx_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer ;
  DMA_Init(FL_SPIx_TX_DMA_STREAM, &DMA_InitStructure);
  
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = FL_SPIx_RX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer; 
  DMA_Init(FL_SPIx_RX_DMA_STREAM, &DMA_InitStructure);

  #else
   SPI_Config();
  #endif
  
}

uint8_t SPI_DMA_Write(const uint8_t * tx_buf, uint32_t tx_size)
{
          uint8_t result = 0;
          uint32_t retry_times = 0;
          
          os_memcpy(dma_buf,  tx_buf, tx_size);
          
          DMA_Cmd(FL_SPIx_TX_DMA_STREAM, DISABLE);    //关闭TX DMA传输 
          while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}	//确保DMA可以被设置  
          FL_SPIx_TX_DMA_STREAM->NDTR = tx_size;
          //FL_SPIx_TX_DMA_STREAM->M0AR = (uint32_t)dma_buf;
          DMA_Cmd(FL_SPIx_TX_DMA_STREAM, ENABLE);   /* Enable DMA SPI TX Stream */

         DMA_Cmd(FL_SPIx_RX_DMA_STREAM, DISABLE);      //关闭RX DMA传输 
         while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}	//确保DMA可以被设置  
         FL_SPIx_RX_DMA_STREAM->NDTR = tx_size;
         //FL_SPIx_RX_DMA_STREAM->M0AR = (uint32_t)tx_buf;
         DMA_Cmd(FL_SPIx_RX_DMA_STREAM, ENABLE);  /* Enable DMA SPI RX Stream */

      
        /* Enable SPI DMA TX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
      
        /* Enable SPI DMA RX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
        
        /* Enable the SPI peripheral */
        SPI_Cmd(FL_SPIx, ENABLE);

         /* Waiting the end of Data transfer */
        retry_times = 50000;
           /* Waiting the end of Data transfer */
        while (DMA_GetFlagStatus(FL_SPIx_TX_DMA_STREAM,  FL_SPIx_TX_DMA_FLAG_TCIF)==RESET)
        {
                  SPI_DMA_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != 0) {
            goto exit;
        }
        
        retry_times = 50000;
        while (DMA_GetFlagStatus(FL_SPIx_RX_DMA_STREAM, FL_SPIx_RX_DMA_FLAG_TCIF)==RESET)
        {
                  SPI_DMA_RETRY_PROCESS(NULL, retry_times, result);
        }
      
exit:
        /* Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(FL_SPIx_TX_DMA_STREAM, FL_SPIx_TX_DMA_FLAG_TCIF);
        DMA_ClearFlag(FL_SPIx_RX_DMA_STREAM, FL_SPIx_RX_DMA_FLAG_TCIF);  
        
        /* Disable DMA SPI TX Stream */
        DMA_Cmd(FL_SPIx_TX_DMA_STREAM, DISABLE);
      
        /* Disable DMA SPI RX Stream */
        DMA_Cmd(FL_SPIx_RX_DMA_STREAM, DISABLE);  
        
        /* Disable SPI DMA TX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Tx, DISABLE);
      
        /* Disable SPI DMA RX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Rx, DISABLE);
      
        /* Disable the SPI peripheral */
        SPI_Cmd(FL_SPIx, DISABLE); 

        return result;
}


uint8_t SPI_DMA_Read(uint8_t * rx_buf,  uint32_t rx_size)
{
          uint8_t result = 0;
          uint32_t retry_times = 0;
          
          os_memset(dma_buf, 0xFF,  rx_size);  // dummy data
          
          DMA_Cmd(FL_SPIx_TX_DMA_STREAM, DISABLE);    //关闭TX DMA传输 
          while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}	//确保DMA可以被设置  
          FL_SPIx_TX_DMA_STREAM->NDTR = rx_size;
          //FL_SPIx_TX_DMA_STREAM->M0AR = (uint32_t)dma_buf;
         DMA_Cmd(FL_SPIx_TX_DMA_STREAM, ENABLE);  /* Enable DMA SPI TX Stream */


         DMA_Cmd(FL_SPIx_RX_DMA_STREAM, DISABLE);      //关闭RX DMA传输 
         while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}	//确保DMA可以被设置  
         FL_SPIx_RX_DMA_STREAM->NDTR = rx_size;
         //FL_SPIx_RX_DMA_STREAM->M0AR = (uint32_t)dma_buf;
        DMA_Cmd(FL_SPIx_RX_DMA_STREAM, ENABLE);   /* Enable DMA SPI RX Stream */

      
        /* Enable SPI DMA TX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
        /* Enable SPI DMA RX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
        
        /* Enable the SPI peripheral */
        SPI_Cmd(FL_SPIx, ENABLE);

        retry_times = 50000;
           /* Waiting the end of Data transfer */
        while (DMA_GetFlagStatus(FL_SPIx_TX_DMA_STREAM,  FL_SPIx_TX_DMA_FLAG_TCIF)==RESET)
        {
                  SPI_DMA_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != 0) {
            goto exit;
        }
        
        retry_times = 50000;
        while (DMA_GetFlagStatus(FL_SPIx_RX_DMA_STREAM, FL_SPIx_RX_DMA_FLAG_TCIF)==RESET)
        {
                  SPI_DMA_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != 0) {
            goto exit;
        }
        os_memcpy(rx_buf, dma_buf, rx_size);
exit:
        /* Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(FL_SPIx_TX_DMA_STREAM, FL_SPIx_TX_DMA_FLAG_TCIF);
        DMA_ClearFlag(FL_SPIx_RX_DMA_STREAM, FL_SPIx_RX_DMA_FLAG_TCIF);  
        
        /* Disable DMA SPI TX Stream */
        DMA_Cmd(FL_SPIx_TX_DMA_STREAM, DISABLE);
      
        /* Disable DMA SPI RX Stream */
        DMA_Cmd(FL_SPIx_RX_DMA_STREAM, DISABLE);  
        
        /* Disable SPI DMA TX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Tx, DISABLE);
      
        /* Disable SPI DMA RX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Rx, DISABLE);
      
        /* Disable the SPI peripheral */
        SPI_Cmd(FL_SPIx, DISABLE); 

        return result;
}

uint8_t SPI_DMA_WR(const uint8_t * tx_buf, uint32_t tx_size, uint8_t * rx_buf,  uint32_t rx_size)
{
          uint8_t result = 0;
          uint32_t retry_times = 0;
          
          os_memcpy(dma_buf,  tx_buf, tx_size);
          os_memset(&dma_buf[tx_size],  0xFF, rx_size);
          
          DMA_Cmd(FL_SPIx_TX_DMA_STREAM, DISABLE);    //关闭TX DMA传输 
          while (DMA_GetCmdStatus(FL_SPIx_TX_DMA_STREAM) != DISABLE){}	//确保DMA可以被设置  
          FL_SPIx_TX_DMA_STREAM->NDTR = tx_size + rx_size;
          FL_SPIx_TX_DMA_STREAM->M0AR = (uint32_t)dma_buf;
          DMA_Cmd(FL_SPIx_TX_DMA_STREAM, ENABLE);   /* Enable DMA SPI TX Stream */

         DMA_Cmd(FL_SPIx_RX_DMA_STREAM, DISABLE);      //关闭RX DMA传输 
         while (DMA_GetCmdStatus(FL_SPIx_RX_DMA_STREAM) != DISABLE){}	//确保DMA可以被设置  
         FL_SPIx_RX_DMA_STREAM->NDTR = rx_size;
         FL_SPIx_RX_DMA_STREAM->M0AR = (uint32_t)tmp_buf;
         DMA_Cmd(FL_SPIx_RX_DMA_STREAM, ENABLE);  /* Enable DMA SPI RX Stream */

      
        /* Enable SPI DMA TX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
        /* Enable SPI DMA RX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
        
        /* Enable the SPI peripheral */
        SPI_Cmd(FL_SPIx, ENABLE);

         /* Waiting the end of Data transfer */
        retry_times = 50000;
           /* Waiting the end of Data transfer */
        while (DMA_GetFlagStatus(FL_SPIx_TX_DMA_STREAM,  FL_SPIx_TX_DMA_FLAG_TCIF)==RESET)
        {
                  SPI_DMA_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != 0) {
            goto exit;
        }
        
        retry_times = 50000;
        while (DMA_GetFlagStatus(FL_SPIx_RX_DMA_STREAM, FL_SPIx_RX_DMA_FLAG_TCIF)==RESET)
        {
                  SPI_DMA_RETRY_PROCESS(NULL, retry_times, result);
        }
         if (result != 0) {
            goto exit;
        }
         os_memcpy(rx_buf, tmp_buf, rx_size);
exit:
        /* Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(FL_SPIx_TX_DMA_STREAM, FL_SPIx_TX_DMA_FLAG_TCIF);
        DMA_ClearFlag(FL_SPIx_RX_DMA_STREAM, FL_SPIx_RX_DMA_FLAG_TCIF);  
        
        /* Disable DMA SPI TX Stream */
        DMA_Cmd(FL_SPIx_TX_DMA_STREAM, DISABLE);
      
        /* Disable DMA SPI RX Stream */
        DMA_Cmd(FL_SPIx_RX_DMA_STREAM, DISABLE);  
        
        /* Disable SPI DMA TX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Tx, DISABLE);
      
        /* Disable SPI DMA RX Requsts */
        SPI_I2S_DMACmd(FL_SPIx, SPI_I2S_DMAReq_Rx, DISABLE);
      
        /* Disable the SPI peripheral */
        SPI_Cmd(FL_SPIx, DISABLE); 

        return result;
}

uint8_t spi_dma_send(const uint8_t * tx_buf, uint32_t tx_size, uint8_t * rx_buf,  uint32_t rx_size)
{
       uint8_t result = 0;  // 0: success

       #if 1
       if(tx_size)
       {
                 result = SPI_DMA_Write(tx_buf, tx_size);
                 if(result)return result;
        }
       if(rx_size)
              result = SPI_DMA_Read(rx_buf, rx_size);
      #else
      result = SPI_DMA_WR(tx_buf, tx_size, rx_buf, rx_size);
      #endif

      return result;
}


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, __IO uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

#if 0
uint8_t  Tx1[36];
//uint8_t Rx1[36];

//uint8_t  Tx2[4096];
uint8_t Rx2[4096];

uint32_t cnt = 0;
void spi_dma_demo(void)
{
        SPI_Config();
        
        for(cnt = 0; cnt < sizeof(Tx1); cnt++)
       {
               Tx1[cnt] =  100 + cnt % 256;
       }
        SPI_DMA_Write(Tx1, sizeof(Tx1));
        if (Buffercmp(Tx1, dma_buf, sizeof(Tx1)) != FAILED) 
       {
              os_printf("spi dma send success \r\n");
       }
       else
       {
               os_printf("spi dma send failed\r\n");
       }


        SPI_DMA_Read(Rx2, sizeof(Rx2));
        if (Buffercmp(Rx2,  dma_buf,  sizeof(Rx2)) != FAILED) 
       {
              os_printf("spi dma send success \r\n");
       }
       else
       {
               os_printf("spi dma send failed\r\n");
       }
}
#endif

