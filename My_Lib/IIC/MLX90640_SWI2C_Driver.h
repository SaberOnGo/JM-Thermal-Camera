#ifndef __MLX90640_SWI2C_Driver_H
#define __MLX90640_SWI2C_Driver_H 

#include "ALL_Includes.h"



int I2CSendByte(int8_t);
void I2CReadBytes(int, char *);
void I2CStart(void);
void I2CStop(void);
void I2CRepeatedStart(void);
void I2CSendACK(void);
void I2CSendNack(void);
int I2CReceiveAck(void);
void Wait(int);
//void IIC_SDA_OUT(void);
//void IIC_SDA_IN(void);
void IIC_GPIO_Init(void);
int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress,uint16_t nMemAddressRead, uint16_t *data);
int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data);

#endif

