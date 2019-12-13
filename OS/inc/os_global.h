
#ifndef __OS_GLOBAL_H
#define __OS_GLOBAL_H

#include "GlobalDef.h"


#define SEC(sec)    ((sec) * (1000L / OS_PER_TICK_MS))   // 设置多少秒


#include <string.h>
#include <stdio.h>
#define  os_memset(src, c, len)         memset((void *)src, c, len)
#define  os_strncpy(dsc, src, len)      strncpy((char *)dsc, (const char *)src, len)
#define  os_strlen(str)                  strlen((char *)str)
#define  os_memcpy(dsc, src, len)       memcpy(dsc, src, len)
#define  os_snprintf(str, size, fmt, ...)        snprintf((char *)str, size, fmt, ##__VA_ARGS__)
#define  os_strchr(s, c)                      strchr((const char *)s, c)  // 查找指定字符在字符串中的第一次出现位置
#define  os_strrchr(s, c)                     strrchr((const char *)s, c)  // 查找指定字符在字符串中的最后一次出现位置
#define  os_strstr(s1, s2)                    strstr((const char * )s1, (const char * )s2 )
#define  os_strncmp(s1, s2, n)                strncmp((const char *)s1, (const char *)s2, (size_t)n)

extern long strtol(const char *str, char **endptr, int base);


/*********** System Tick Clock Function  bgein  ***************************/
#define SysTick_Open()  SysTick->CTRL |=  (SysTick_CTRL_ENABLE_Msk)
#define SysTick_Close() SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk)


extern uint8_t volatile flag10ms;
extern uint8_t volatile flag100ms;
extern uint8_t volatile flag_switch;
extern volatile uint16_t key_result;

void SysTick_Increment(void);
void SysTick_Init(void);
uint32_t os_get_tick(void);
#define  os_get_sec()       (os_get_tick() / 100)
/*********** System Tick Clock Function  end  ***************************/


#endif

