#include "ALL_Includes.h"
#include "delay.h"

/*
u16 fac_ms;//全局变量
u8 fac_us;//全局变量

__IO u16 ntime;	
							    
void delay_ms(u16 nms)
{
   	  SysTick->LOAD = (u32)fac_ms*nms-1;//加载时间值
	  SysTick->VAL = 1;//随便写个值，清除加载寄存器的值
	  SysTick->CTRL |= BIT(0);//SysTick使能
	  while(!(SysTick->CTRL&(1<<16)));//判断是否减到0
	  SysTick->CTRL &=~BIT(0);//关闭SysTick
}

void Delay_Init(u8 SYSCLK)
{
        SysTick->CTRL &=~BIT(2);//选择外部时钟
	 SysTick->CTRL &=~BIT(1);//关闭定时器减到0后的中短请求
	 fac_us = SYSCLK/8;//计算好SysTick加载值
	 fac_ms = (u16)fac_us*1000;	 
}

		    								   
void delay_us(u32 nus)
{		
	  SysTick->LOAD = (u32)fac_us*nus-1;//加载时间值
	  SysTick->VAL = 1;//随便写个值，清除加载寄存器的值
	  SysTick->CTRL |= BIT(0);//SysTick使能
	  while(!(SysTick->CTRL&(1<<16)));//判断是否减到0
	  SysTick->CTRL &=~BIT(0);//关闭SysTick
}
*/


#define DELAY_TIM_FREQUENCY_US 1000000		/* = 1MHZ -> timer runs in microseconds */
#define DELAY_TIM_FREQUENCY_MS 1000			/* = 1kHZ -> timer runs in milliseconds */

uint32_t  PCLK1_CLK;  

#define TIM3_TimeBaseInit(TIMx, TIM_TimeBaseInitStruct) {\
	TIMx->CR1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));\
	TIMx->CR1 |= (uint32_t)TIM_TimeBaseInitStruct.TIM_CounterMode;\
	TIMx->ARR = TIM_TimeBaseInitStruct.TIM_Period;\
	TIMx->PSC = TIM_TimeBaseInitStruct.TIM_Prescaler;\
	TIMx->EGR = TIM_PSCReloadMode_Immediate;\
	}

// Init timer for Microseconds delays
#if 0
void _init_us(void) 
{
	TIM_TimeBaseInitTypeDef TIM;

	// Enable clock for TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	// Time base configuration
	TIM_TimeBaseStructInit(&TIM);
	TIM.TIM_Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_US)-1;
	TIM.TIM_Period = UINT16_MAX;
	TIM.TIM_ClockDivision = 0;
	TIM.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM);

	// Enable counter for TIM2
	TIM_Cmd(TIM2,ENABLE);
}
#else



#if 1
#define _init_us() {\
       (RCC->APB1ENR |= RCC_APB1Periph_TIM3);\
       TIM3->PSC  = (PCLK1_CLK  / DELAY_TIM_FREQUENCY_US) - 1;\
       TIM3->ARR  = UINT16_MAX;\
	TIM3->EGR  = TIM_PSCReloadMode_Immediate;\
	TIM3->CR1 |= TIM_CR1_CEN;\
	}
#else
#define _init_us() {\
	TIM_TimeBaseInitTypeDef TIM;\
	SET_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_TIM2);\
	TIM.TIM_Prescaler = (SystemCoreClock / DELAY_TIM_FREQUENCY_US) - 1;\
	TIM.TIM_Period = UINT16_MAX;\
	TIM.TIM_ClockDivision = 0;\
	TIM.TIM_CounterMode = TIM_CounterMode_Up;\
	TIM2_TimeBaseInit(TIM2, TIM);\
	SET_REG_32_BIT(TIM2->CR1, TIM_CR1_CEN);\
	}
#endif

#endif

// Init and start timer for Milliseconds delays
#if 0
void _init_ms(void) 
{

    TIM_TimeBaseInitTypeDef TIM;
	
	// Enable clock for TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	// Time base configuration
	
	TIM_TimeBaseStructInit(&TIM);
	TIM.TIM_Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_MS)-1;
	TIM.TIM_Period = UINT16_MAX;
	TIM.TIM_ClockDivision = 0;
	TIM.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM);

	// Enable counter for TIM2
	TIM_Cmd(TIM2,ENABLE);
}
#else
#define _init_ms() {\
	(RCC->APB1ENR |= RCC_APB1Periph_TIM3);\
	TIM3->PSC  = (SystemCoreClock / DELAY_TIM_FREQUENCY_MS) - 1;\
	TIM3->ARR  = UINT16_MAX;\
	TIM3->EGR  = TIM_PSCReloadMode_Immediate;\
	TIM3->CR1 |= TIM_CR1_CEN;\
	}
#endif

// Stop timer
#if 0
void _stop_timer(void) {
	TIM_Cmd(TIM2,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE); // Powersavings?
}
#else
#define _stop_timer() {\
	CLEAR_REG_32_BIT(TIM3->CR1, TIM_CR1_CEN);\
	CLEAR_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_TIM3);\
	}
#endif


// Do delay for nTime milliseconds
void delay_ms(uint32_t mSecs) 
{
       #if 0
        uint32_t start;
   
	// Init and start timer
	_init_ms();

	// Dummy loop with 16 bit count wrap around
	TIM2->CNT = 0;
	start = TIM2->CNT;
	while((TIM2->CNT-start) <= mSecs);

	// Stop timer
	_stop_timer();
	#else
	mSecs <<= 1;
	while(mSecs--)
	{
              delay_us(1000);
       }
	#endif
}


// Do delay for nTime microseconds
WEAK_ATTR void delay_us(uint32_t uSecs) 
{
    volatile uint32_t start = 0;
	
	// Init and start timer
	_init_us();

       
	// Dummy loop with 16 bit count wrap around
	//start = TIM2->CNT;
	TIM3->CNT = 0;
	while((TIM3->CNT-start) <= uSecs);

	// Stop timer
	_stop_timer();
}
