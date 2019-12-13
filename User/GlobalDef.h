
#ifndef __GLOBAL_DEF_H
#define __GLOBAL_DEF_H

#include "stm32f4xx_conf.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "reglib.h"

#define  DEBUG_EN  0

#if  DEBUG_EN
#define os_printf(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define  os_printf(...)
#endif


// debug output enable : 0: disable;  1: enable
#define    FAT_DEBUG_EN   0    // fatfs debug output






#define OS_PER_TICK_MS   (10)         // 每个tick的时间: 10ms


#define  SD_DISK_EN            0   // SD 卡存储使能
#define  FLASH_DISK_EN    1   // SPI FLASH DISK 存储使能(1)
#define  ROM_DISK_EN       0  // ROM FLASH DISK 存储使能

#define  MAX_LUN         0    // MAX_LUN + 1 个可移动磁盘 :   SD卡 + SPI FLASH

typedef enum
{
   FLASH_DISK = 0,
   SD_DISK       = 1, 
   ROM_DISK    = 2,
}DISK_ENUM;

typedef enum
{
   APP_SUCCESS = 0,
   APP_FAILED = 1,
   APP_NULL = 0xFF,
}E_RESULT;


#define FREQ_512KHz       (512000L)
#define FREQ_1MHz        (1000000L)
#define FREQ_2MHz        (2000000L)
#define FREQ_8MHz        (8000000L)
#define FREQ_16MHz      (16000000L)
#define FREQ_24MHz      (24000000L)
#define FREQ_48MHz      (48000000L)
#define FREQ_72MHz      (72000000L)


#define CPU_CLOCK       FREQ_72MHz  // MCU 时钟


/************************************************************************************************************************************************************
 spi flash 物理扇区大小为4KB, 物理总扇区数为512, 容量 2MB
 其中 260KB 空间 为 应用程序和系统环境变量存储区
| <---sector 0 - sector 63--->  | <-----------sector 64 ~ 127-----------> | <--sector 128 ----- >  |<-sector 65 ~ sector 511-------->   |
| <-----256 KB -----------> | <--------256 KB--------------------> | <------4KB------->|---------文件系统---------- -|
| APP1 备份区(新程序)     |  APP2 备份区(上一个版本程序)    |   系统参数区        |---------文件系统----------- |  
|-------------------------- 516 KB (129个物理扇区)    --------------------------------|--剩余为 FatFs 文件系统区---|
*************************************************************************************************************************************************************/
// APP1 BIN 备份地址: 256 KB, sector 0 - sector 63
#define FLASH_APP1_START_SECTOR  ((uint32_t)0L)

// APP2 BIN 备份地址: 256 KB, sector 64 - sector 127
#define FLASH_APP2_START_SECTOR  ((uint32_t)64L)

// 系统环境参数起始扇区地址, sector 128 扇区, 大小 4KB
#define  FLASH_SYS_ENV_START_SECTOR    ((uint32_t)128L) 

// 字体描述信息起始地址
//#define  FLASH_FONT_START_SECTOR (FLASH_SYS_ENV_START_SECTOR + 1)  //(FLASH_SYS_ENV_START_SECTOR + 1)

// 文件系统起始地址
#define  FLASH_FATFS_START_SECTOR    (FLASH_SYS_ENV_START_SECTOR + 1)   //(FLASH_FONT_START_SECTOR + 128)










#define WEAK_ATTR   __attribute__((weak))

#define READ_REG_32_BIT(reg,  b)      ((uint32_t)((reg)&((uint32_t)b)))      //b必须为整数, reg为32 bit 寄存器
#define CLEAR_REG_32_BIT(reg, b)      ((reg)&=(uint32_t)(~((uint32_t)b)))   //b必须为整数, reg为32 bit 寄存器
#define SET_REG_32_BIT(reg,   b)      ((reg)|=(uint32_t)(b))                  //b必须为整数, reg为32 bit 寄存器

#define READ_REG_8_BIT(reg,b)     ((uint8_t)((reg)&((uint8_t)b)))           //b必须为8bit整数, reg为寄存器
#define CLEAR_REG_8_BIT(reg,b)    ((reg)&=(uint8_t)(~((uint8_t)b)))        //b必须为8bit整数, reg为寄存器
#define SET_REG_8_BIT(reg,b)      ((reg)|=(uint8_t)(b))                      //b必须为8bit整数, reg为寄存器


#define ALIGN_4  __align(4)

#define  ALIGN4(len)   ( (sizeof(len) + sizeof(int) - 1) &  ~(sizeof(int) - 1) ) 

// 按4字节对齐后的长度
#define  ALIGN4_SIZE(len)   ( ( len & 0xFFFFFFFC) + ((len % 4) ?  4 : 0))

// 按4字节对齐时缺少的字节长度: 0 - 3
#define ALIGN4_LEFT(len)     ((len % 4) ? (4 - len % 4) : 0)


#endif

