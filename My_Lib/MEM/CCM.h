
#ifndef  __CCM_DEF_H
#define  __CCM_DEF_H

#include <stdint.h>
#include "GlobalDef.h"

#define   ALIGN_32 __align(32)   

#define   CCM_ATTR    __attribute__((at(0X10000000)))	// CCM ÄÚ´æÉùÃ÷


#define CCM_BUF_SIZE  65280  //65280L   // 960 * 64 = 65280 < 64K = 65536

#define USE_CCM_RAM  1 // 0: use normal RAM; 1: use CCM RAM(max 64 KB)



extern ALIGN_32 
uint8_t ccm_buf[CCM_BUF_SIZE] 
#if USE_CCM_RAM
CCM_ATTR
#endif
;


#endif

