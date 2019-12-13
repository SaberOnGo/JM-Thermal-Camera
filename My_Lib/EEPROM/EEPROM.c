#include "EEPROM.h"
#include "ALL_Includes.h"

/******************************
EEPROM 内存规划，相对地址
0--- 0xD5A5//用于首次写入标记
1--- TempH  //高温
2--- TempL //低温
3--- emissivity  //反射率

******************************/

/*******************************
EEP_Addr: 相对地址
EEP_Buf： 内容
EEP_Length: 数据长度(16bit 为1个长度)
*******************************/
void EEP_Wirte(u16 *EEP_Buf, u16 EEP_Length)
{
	 u16 EEP_i;

	 FLASH_Unlock();	

	 FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);//搽除

	 for(EEP_i = 0; EEP_i < EEP_Length; EEP_i++)//写入EEP
	 {
				FLASH_ProgramHalfWord(EEP_BaseAddr +(EEP_i << 1), EEP_Buf[EEP_i]);
	 }

	 FLASH_Lock();
}

u16 EEP_ReadWord(u32 EEP_Addr)
{
	 return *(__IO uint16_t *)(EEP_BaseAddr + (EEP_Addr<<1));
}



void ReadDataFromEEP(u16 addr, u8 *data, u16 length)
{
	 u8 i;
	 u16 temp;

	 for(i = 0; i < length; i++)
	 {
		  temp = EEP_ReadWord(i+addr);
	  	  data[i] = (u8)temp;
	 }
}

