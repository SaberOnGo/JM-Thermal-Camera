#ifndef  _EEPROM_H


#include "ALL_Includes.h"

#define  EEP_BaseAddr         0x8060000
#define  TempFlagAddr         0
#define  HightTempAddr        1
#define  LowTempAddr          2
#define  EmissivityAddr       3
#define  AutoRuleAddr         4
#define  EepWrFlag            0xD5A5

void EEP_Wirte(u16 *EEP_Buf, u16 EEP_Length);
u16   EEP_ReadWord(u32 EEP_Addr);
void ReSerialNumberFromEEP(u8 * SerNumber);
void ReadDataFromEEP(u16 addr, u8 *data, u16 length);
























#endif

