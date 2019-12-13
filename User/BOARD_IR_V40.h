
#ifndef  __BOARD_IR_V40_H
#define  __BOARD_IR_V40_H

#include "stm32f4xx_conf.h"
#include "reglib.h"


/*********************** TFT 屏IO 定义 begin ****************************/
#define FSMC_CS_PORT    GPIOD
#define FSMC_CS_Pin          GPIO_Pin_7
#define FSMC_CS_L()          IO_L(FSMC_CS_PORT, FSMC_CS_Pin)
#define FSMC_CS_H()         IO_H(FSMC_CS_PORT, FSMC_CS_Pin)

#define  TFT_BLK_PORT   	               GPIOC
#define  TFT_BLK_Pin                          GPIO_Pin_11
#define  TFT_BackLight_Open()    IO_L(TFT_BLK_PORT,  TFT_BLK_Pin)
#define  TFT_BackLight_Close()    IO_H(TFT_BLK_PORT, TFT_BLK_Pin)

#define TFT_RST_PORT   	 GPIOB
#define TFT_RST_Pin             GPIO_Pin_15
#define TFT_RST_L                 IO_L(TFT_RST_PORT, TFT_RST_Pin)
#define TFT_RST_H                IO_H(TFT_RST_PORT, TFT_RST_Pin) 

/*********************** TFT 屏IO 定义 end ****************************/


/*********************** 按键定义 begin ****************************/
// UP KEY
#define  KeyUp_Port         GPIOC
#define  KeyUp_Pin            GPIO_Pin_12

// DOWN KEY,  无此按键
#define  KeyDown_Port   GPIOA
#define  KeyDown_Pin      GPIO_Pin_0

// SET KEY
#define  KeySet_Port         GPIOB
#define  KeySet_Pin            GPIO_Pin_13

// 电源检测管脚/ 电源按键
#define  KeyDect_Port        GPIOE
#define  KeyDect_Pin          GPIO_Pin_0
#define  KeyDect_State     (KeyDect_Port->IDR & KeyDect_Pin) 


#define  BatCHRG_Port      GPIOB
#define  BatCHRG_Pin         GPIO_Pin_8
#define  BatCHRG_State    (BatCHRG_Port->IDR & BatCHRG_Pin)

#define  BatSTDBY_Port     GPIOB
#define  BatSTDBY_Pin        GPIO_Pin_9
#define  BatSTDBY_State   (BatSTDBY_Port->IDR & BatSTDBY_Pin)

//#define BatCE_PORT            GPIOC
//#define BatCE_Pin                  GPIO_Pin_5
//#define BatCE_H()                 IO_H(BatCE_PORT,  BatCE_Pin)
//#define BatCE_L()                 IO_L(BatCE_PORT,  BatCE_Pin)

// 电源开关控制IO
#define  Power_Port    GPIOB
#define  Power_Pin       GPIO_Pin_3
#define  PowerOFF       IO_L(Power_Port,  Power_Pin)
#define  PowerON         IO_H(Power_Port,  Power_Pin)

// ADC 控制管脚
#define   ADC_Ctrl_Port          GPIOE
#define   ADC_Ctrl_Pin            GPIO_Pin_2
#define   ADC_Ctrl_Open()   IO_H(ADC_Ctrl_Port,  ADC_Ctrl_Pin)
#define   ADC_Ctrl_Close()   IO_L(ADC_Ctrl_Port,  ADC_Ctrl_Pin)

// 电池电压测量管脚
#define  BAT_ADC_PORT       GPIOC
#define  BAT_ADC_Pin             GPIO_Pin_0
#define  BAT_ADC_Channel   ADC_Channel_0

// USB 插入检测
//#define  VIN_DETECT_PORT   GPIOB
//#define  VIN_DETECT_Pin         GPIO_Pin_13



// USB 数据通信控制-> PD3
#define USE_VBUS_Ctrl   0   
#define  VBUS_Ctrl_PORT          
#define  VBUS_Ctrl_Pin                
#define  VBUS_Ctrl_H()               IO_H(VBUS_Ctrl_PORT,  VBUS_Ctrl_Pin)
#define  VBUS_Ctrl_L()                IO_L(VBUS_Ctrl_PORT,  VBUS_Ctrl_Pin)
#define  VBUS_Ctrl_Open()         __NOP() //VBUS_Ctrl_H()
#define  VBUS_Ctrl_Close()        __NOP() //VBUS_Ctrl_L()




// 电源保持管脚
//#define  Power_Keep_PORT      GPIOB
//#define  Power_Keep_Pin            GPIO_Pin_3
//#define  Power_Keep_Open()   IO_H(Power_Keep_PORT,  Power_Keep_Pin)
//#define  Power_Keep_Close()   IO_L(Power_Keep_PORT,  Power_Keep_Pin)

//#define  SD_DET_PORT      GPIOE
//#define  SD_DET_Pin            GPIO_Pin_2
//#define  SD_DET_Read()   IO_READ_IN(SD_DET_PORT, SD_DET_Pin)

/*********************** 按键定义 end ****************************/


/******************* MLX90640   IIC 接口定义 begin *************************/
#define SCL_PORT   	 	      GPIOC
#define SCL_Pin                       GPIO_Pin_6
#define SCL_LOW                   IO_L(SCL_PORT, SCL_Pin)
#define SCL_HIGH                  IO_H(SCL_PORT, SCL_Pin)

#define SDA_PORT   	       GPIOC
#define SDA_Pin                       GPIO_Pin_7
#define SDA_LOW                   IO_L(SDA_PORT, SDA_Pin)
#define SDA_HIGH                  IO_H(SDA_PORT, SDA_Pin)
#define sda                                  (SDA_PORT->IDR & SDA_Pin) 

#define IIC_SDA_OUT()      SDA_PORT->MODER |= 0X4000  // 输出
#define IIC_SDA_IN()	       SDA_PORT->MODER &=0XFFFF3FFF //输入

/********************* MLX90640   IIC 接口定义 end **************************/


#endif

