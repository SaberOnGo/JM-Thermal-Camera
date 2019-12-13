#ifndef __USART_H
#define __USART_H 

#include "ALL_Includes.h"

extern u8 UsartModule;

void uart_init(u32 bound);
void USART_SendBye(u8 data);

#endif

