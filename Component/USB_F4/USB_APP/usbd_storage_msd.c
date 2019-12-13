#include "usbd_msc_mem.h"
#include "usb_conf.h"
#include "GlobalDef.h" 
#if FLASH_DISK_EN
#include "spi_flash_interface.h"
#endif

#if ROM_DISK_EN
#include "rom_flash_interface.h"
#endif


#if SD_DISK_EN
#include "sd_card_interface.h"
#endif



////////////////////////////自己定义的一个标记USB状态的寄存器///////////////////
//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)
vu8 USB_STATUS_REG = 0;



/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = {   // 36
   
	/* LUN 0 */ 
	0x00,		
	0x80,		
	0x02,		
	0x02,
	(USBD_STD_INQUIRY_LENGTH - 5),
	0x00,
	0x00,	
	0x00,
    /* Vendor Identification */
    'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
    /* Product Identification */
    'S', 'P', 'I', 'F', 'L', 'A', 'S', 'H', ' ', /* Product : 16 Bytes */
    ' ','D', 'i', 's', 'k', ' ',
    /* Product Revision Level */	
    '1', '.', '0', ' ',							/* Version : 4 Bytes */	
	
	/* LUN 1 */
	0x00,
	0x80,		
	0x02,		
	0x02,
	(USBD_STD_INQUIRY_LENGTH - 5),
	0x00,
	0x00,	
	0x00,
	/* Vendor Identification */
	'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
	/* Product Identification */				
	'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',  /* Product : 16 Bytes */
	'D', 'i', 's', 'k', ' ', ' ',  
    /* Product Revision Level */
	'1', '.', '0' ,' ',                      	/* Version : 4 Bytes */
}; 
int8_t STORAGE_Init (uint8_t lun);
int8_t STORAGE_GetCapacity (uint8_t lun,uint32_t *block_num,uint32_t *block_size);
int8_t  STORAGE_IsReady (uint8_t lun);
int8_t  STORAGE_IsWriteProtected (uint8_t lun);
int8_t STORAGE_Read (uint8_t lun,uint8_t *buf, uint32_t blk_addr,uint16_t blk_len);
int8_t STORAGE_Write (uint8_t lun,uint8_t *buf, uint32_t blk_addr,uint16_t blk_len);
int8_t STORAGE_GetMaxLun (void);

//USB Device 用户回调函数接口
USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =
{
	STORAGE_Init,
	STORAGE_GetCapacity,
	STORAGE_IsReady,
	STORAGE_IsWriteProtected,
	STORAGE_Read,
	STORAGE_Write,
	STORAGE_GetMaxLun,
	(int8_t *)STORAGE_Inquirydata,
};
USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;

//初始化存储设备
//lun:逻辑单元编号,0,SD卡;1,SPI FLASH
//返回值:0,成功;
//    其他,错误代码
int8_t STORAGE_Init (uint8_t lun)
{
       int8_t status  = 0;
       
	#if FLASH_DISK_EN
        status = (int8_t)flash_initialize();
	#endif

	#if SD_DISK_EN
        status = (int8_t)sd_initialize();
	#endif

	#if ROM_DISK_EN
        status = rom_flash_initialize();
	#endif

	return status;
} 

//获取存储设备的容量和块大小
//lun:逻辑单元编号: 0 -2
//block_num:块数量(扇区数)
//block_size:块大小(扇区大小)
//返回值:0,成功;
//    其他,错误代码
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{     
	switch(lun)
     {
          #if FLASH_DISK_EN
          case FLASH_DISK:  // SPI FLASH
	  {
                *block_num = SPI_FLASH_SECTOR_COUNT; 
                *block_size  = SPI_FLASH_SECTOR_SIZE; 
	  }break;
	  #endif

	  #if SD_DISK_EN
	  case SD_DISK:  // SD 卡
	  {
	          *block_num = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize;
                 *block_size  = SDCardInfo.CardBlockSize;
	  }break;
	  #endif

	  #if ROM_DISK_EN
	  case ROM_DISK:   // ROM FLASH
	  {
		 *block_num = ROM_FLASH_SIZE / ROM_FLASH_PAGE_SIZE; 
               *block_size  =  ROM_FLASH_PAGE_SIZE; 
	  }break;
	  #endif
	  
	  default:
	  	return 1;
     }
     return 0;
} 

//查看存储设备是否就绪
//lun:逻辑单元编号,0,SD卡;1,SPI FLASH
//返回值:0,就绪;
//    其他,未就绪
int8_t  STORAGE_IsReady (uint8_t lun)
{ 
	USB_STATUS_REG |= 0X10;//标记轮询
	return 0;
} 

//查看存储设备是否写保护
//lun:逻辑单元编号,0,SD卡;1,SPI FLASH
//返回值:0,没有写保护;
//    其他,写保护(只读)
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
	return  0;
} 

//从存储设备读取数据
//lun:逻辑单元编号
//buf:数据存储区首地址指针
//sector_addr:要读取的地址(扇区地址)
//sector_cnt:要读取的块数(扇区数) 
//返回值:0,成功;
//    其他,错误代码 
int8_t STORAGE_Read (uint8_t lun, uint8_t * Readbuff,uint32_t  sector_addr, uint16_t sector_cnt)
{
           int8_t res = 0;

          USB_STATUS_REG |= 0X02; // mask reading
          switch (lun)
         {
#if FLASH_DISK_EN
                 case FLASH_DISK:
           	  {
           		res = flash_mal_read(lun, sector_addr << 9, (uint32_t *)Readbuff,  sector_cnt << 9);
           	   }break;
#endif
           
#if SD_DISK_EN
                 case SD_DISK:
           	  {
           	           res = SD_ReadDisk(lun, Readbuff,  sector_addr,  sector_cnt);
           	   }break;
#endif
           
#if ROM_DISK_EN
           	  case ROM_DISK:
             	 {
           		res = rom_flash_mal_read(lun, sector_addr << 9, (uint32_t *)Readbuff,  sector_cnt << 9);
               }break;
#endif
           	
               default:
                     return 1;
          }
          if(res)
          {
                     if(res)USB_STATUS_REG |= 0X08;  // read error
                     return 1;
          }
          return 0;
}

//向存储设备写数据
//lun:逻辑单元编号,0,SD卡;1,SPI FLASH
//buf:数据存储区首地址指针
//blk_addr:要写入的地址(扇区地址)
//blk_len:要写入的块数(扇区数) 
//返回值:0,成功;
//    其他,错误代码 
int8_t STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) 
{
       int8_t res = 0; 

       USB_STATUS_REG |= 0X01; // mask on writing
	switch(lun)  //这里,根据lun的值确定所要操作的磁盘
	{
	       #if FLASH_DISK_EN
	       case FLASH_DISK:  //磁盘0为 SPI FLASH盘
		{
		        res = (int8_t)flash_mal_write(lun, blk_addr << 9,  (uint32_t *)buf,  blk_len << 9);
		}break;
		#endif

		#if SD_DISK_EN
		case SD_DISK:  //磁盘1为SD卡	
		{
		         res = (int8_t)SD_WriteDisk(lun, buf,  blk_addr, blk_len);
		}break;
		#endif

		#if ROM_DISK_EN
		case ROM_DISK:  
	      {
			 res = (int8_t)rom_flash_mal_write(lun,  blk_addr << 9,  (uint32_t *)buf,  blk_len << 9);
	       }break;
	      #endif
		
		default:
			return 1;
	}
	if(res)
	{
	          USB_STATUS_REG |= 0X04; // mask write error
	          return 1;
	}
	return 0;
}

//获取支持的最大逻辑单元个数
//返回值:支持的逻辑单元个数-1
int8_t STORAGE_GetMaxLun (void)
{
	return MAX_LUN;
}
























