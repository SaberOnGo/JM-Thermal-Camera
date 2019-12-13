
#include "os_global.h"

#include "GlobalDef.h"

uint8_t volatile flag10ms = 0;
uint8_t volatile flag_switch = 0;

void SysTick_Init(void)
{
    if(SysTick_Config(SystemCoreClock / (1000 / OS_PER_TICK_MS)))  // 10 ms ÷–∂œ“ª¥Œ
    {
        while(1);
    }

}

#if (! SYS_RTOS_EN)
static volatile uint32_t sSysTick = 0;




void SysTick_Increment(void)
{
   sSysTick++;
   flag10ms = 1;  

   
   flag_switch ^= 1;
}
#endif

uint32_t os_get_tick(void)
{
   #if SYS_RTOS_EN
   return OSTimeGet();
   #else
   return sSysTick;
   #endif
}

