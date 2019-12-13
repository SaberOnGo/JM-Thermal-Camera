
#ifndef __REG_LIB_H__
#define  __REG_LIB_H__
/********************** STM32F40X ¼Ä´æÆ÷¿â ******************************************/


#define  IO_H(GPIOx, Pin)          (GPIOx->BSRRL   = Pin)
#define  IO_L(GPIOx, Pin)          (GPIOx->BSRRH   = Pin)
#define  IO_READ_IN(GPIOx, Pin)    ( (GPIOx->IDR & Pin) ? 1 : 0)
#define IO_READ_OUT(GPIOx, Pin)   ( (GPIOx->ODR & Pin) ? 1 : 0)

#endif

