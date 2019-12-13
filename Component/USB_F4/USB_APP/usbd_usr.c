
#include "usbd_usr.h" 
#include "usb_dcd_int.h"
#include <stdio.h> 
#include "GlobalDef.h"

// connect status: 0: no conn; 1: connected
vu8 bDeviceState = 0;		// default: no connect



extern USB_OTG_CORE_HANDLE  USB_OTG_dev;

extern uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
extern uint32_t USBD_OTG_EP1IN_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);
#endif


#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler(&USB_OTG_dev);
}
#endif
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler(&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler(&USB_OTG_dev);
}
#endif


USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

void USBD_USR_Init(void)
{
	
} 

//USB Device  Reset
//speed:  0: high speed; 1: full speed; >1: others
void USBD_USR_DeviceReset (uint8_t speed)
{
	switch (speed)
	{
		case USB_OTG_SPEED_HIGH:
			os_printf("USB Device Library v1.2.1  [HS]\r\n");
			break; 
		case USB_OTG_SPEED_FULL: 
			os_printf("USB Device Library v1.2.1  [FS]\r\n");
			break;
		default:
			os_printf("USB Device Library v1.2.1  [??]\r\n"); 
			break;
	}
}

void USBD_USR_DeviceConfigured (void)
{
	os_printf("MSC Interface started.\r\n"); 
} 

void USBD_USR_DeviceSuspended(void)
{
	os_printf("Device In suspend mode.\r\n");
} 

void USBD_USR_DeviceResumed(void)
{ 
	os_printf("Device Resumed\r\n");
}


void USBD_USR_DeviceConnected (void)
{
	bDeviceState = 1;
	os_printf("USB Device Connected.\r\n");
}

void USBD_USR_DeviceDisconnected (void)
{
	bDeviceState = 0;
	os_printf("USB Device Disconnected.\r\n");
} 


















