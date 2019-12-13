
#include "usb_bsp.h" 
#include "sys.h"  
#include "delay.h" 
#include "board_version.h"


   
#if 0
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
       GPIO_InitTypeDef  GPIO_InitStructure;
     
       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);       // 使能GPIOA时钟
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);   // 使能USB OTG时钟
       
       //GPIOA11,A12设置
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;  //PA11/12复用功能输出	
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    // 复用功能
       GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               // 推挽输出
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   // 100MHz
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
       GPIO_Init(GPIOA, &GPIO_InitStructure);   // 初始化
     	
     	
       GPIO_InitStructure.GPIO_Pin = VBUS_Ctrl_Pin;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
       GPIO_Init(VBUS_Ctrl_PORT, &GPIO_InitStructure);
     	
       VBUS_Ctrl_Open();  // open usb host power

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG_FS);  // PA11,AF10(USB)
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG_FS);  // PA12,AF10(USB)
}
#endif

/**
* @brief  USB_OTG_BSP_Init
*         Initializes BSP configurations
* @param  None
* @retval None
*/

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE * pdev)
{

  GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USE_USB_OTG_FS

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Configure SOF ID DM DP Pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_OTG1_FS);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG1_FS);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG1_FS);

  /* Configure VBUS Pin */
  #if USE_VBUS_Ctrl
  GPIO_InitStructure.GPIO_Pin = VBUS_Ctrl_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(VBUS_Ctrl_PORT, &GPIO_InitStructure);
  VBUS_Ctrl_Open();  // open usb host power
  #endif
  
  /* Configure ID pin */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  //GPIO_Init(GPIOA, &GPIO_InitStructure);
  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_OTG1_FS);


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);
#else                           /* USE_USB_OTG_HS */
      #error "have to have extern phy chip to enable USB OTG HS \r\n"
#endif                          /* USB_OTG_HS */
}


void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
      NVIC_InitTypeDef NVIC_InitStructure;

     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   #ifdef USE_USB_OTG_HS
     NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
   #else
     NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
   #endif
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
     
   #ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
     NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_OUT_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
   
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
     NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_IN_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
   #endif
}


void USB_OTG_BSP_DisableInterrupt(void)
{ 
}

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{ 

}

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{ 
} 


/* This value is set for SYSCLK = 168 MHZ, User can adjust this value
     * depending on used SYSCLK frequency */
#define count_us   55

void USB_OTG_BSP_uDelay(const uint32_t usec)
{
  uint32_t count = count_us * usec;
  do
  {
    if (--count == 0)
    {
      return;
    }
  }
  while (1);
}


//USB_OTG ms级延时函数
//本例程采用SYSTEM文件夹的delay.c里面的delay_ms函数实现
//官方例程采用的是定时器2来实现的.
//msec:要延时的ms数.
void USB_OTG_BSP_mDelay (const uint32_t msec)
{  
	delay_ms(msec);
}
   




