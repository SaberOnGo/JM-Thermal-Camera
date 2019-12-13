#ifndef __KEY_H
#define __KEY_H 

#include "ALL_Includes.h"

//extern u8 DispState;
extern u8 emissivity;
extern u8 RuleAuto;	



#define  KeySetOn            0x00
#define  KeyDownOn      0x01
#define  KeyUpOn             0x02
#define  KeyPowerOn     0x03
#define  NoKeyOn              0xff


void KeyScan(void);
void MX_GPIO_Init(void);
u8 GetKeyBit(void);
void SetTemp(void);
void KeyAction(void);
void SetDownTemp(void);
void SetUpTemp(void);	
void SetEmissivityUp(void);
void SetEmissivityDown(void);
void SetAutoRuleUpDown(void);

#endif

