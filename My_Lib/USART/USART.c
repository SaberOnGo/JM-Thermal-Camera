#include "USART.h"
#include "ALL_Includes.h"

u8 UsartModule = DISABLE;

void uart_init(u32 bound)
{
       GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  
  	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 /*| GPIO_Pin_3*/;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);    

  	USART_InitStructure.USART_BaudRate = bound;//²¨ÌØÂÊÉèÖÃ
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Tx; 
  	
  	USART_Init(USART2, &USART_InitStructure);
#if DEBUG_EN
 	USART_Cmd(USART2, ENABLE);
#endif
  	USART_ClearFlag(USART2, USART_FLAG_TC);
}

void USART_SendBye(u8 data)
{
	 while((USART2->SR&0X40)==0);
	 USART2->DR = data;  
}

#if DEBUG_EN
#include <stdio.h>

int fputc(int ch, FILE *f)
{      
      while((USART2->SR&0X40)==0);
	 USART2->DR = ch;  
	 
	return ch;
}
#endif

