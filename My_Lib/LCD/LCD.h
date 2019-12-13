#ifndef __lcd_H
#define __lcd_H 

#include "ALL_Includes.h"
#include "reglib.h"


#define Bank1_LCD_D    ((u32)0x60020000)    //disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	 //disp Reg ADDR

#define LLCD_WRITE_CMD(cmd)              (*(volatile u16 *) (Bank1_LCD_C)= ((u16)cmd))
#define LLCD_WRITE_DATA(dataa)		 (*(volatile u16 *) (Bank1_LCD_D)= ((u16)dataa))
#define LLCD_READ_DATA()                         ( *(__IO uint16_t *)Bank1_LCD_D)

#define LCD_ILI9341_CMD(index)                 LLCD_WRITE_CMD(index)
#define LCD_ILI9341_Parameter(val)	 LLCD_WRITE_DATA(val)


#define LCD_WidthX  X_MAX_PIXEL
#define LCD_WidthY  Y_MAX_PIXEL




//画笔颜色
#define WHITE      0xFFFF
#define BLACK      0x0000	  
#define BLUE       0x001F  
#define BRED       0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED        0xF800
#define MAGENTA    0xF81F
#define GREEN      0x07E0
#define CYAN       0x7FFF
#define YELLOW     0xFFE0
#define BROWN 		 0XBC40 //棕色
#define BRRED 		 0XFC07 //棕红色
#define GRAY  		 0X8430 //灰色

//扫描方向定义
typedef enum
{
     L2R_U2D  = 0,  // 从左到右,从上到下
     L2R_D2U  = 1,  // 从左到右,从下到上
     R2L_U2D  = 2,  // 从右到左,从上到下
     R2L_D2U  = 3,  // 从右到左,从下到上

     U2D_L2R  = 4,  // 从上到下,从左到右
     U2D_R2L  = 5,  // 从上到下,从右到左
     D2U_L2R  =6,  // 从下到上,从左到右
     D2U_R2L = 7,  // 从下到上,从右到左	 
}E_LCD_SCAN_DIR;

void LCD_init(void);
void LCD_setwindow(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);
void LCD_GPIO_Init(void);
void LCD_Reset(void);
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
u16 LCD_RD_DATA(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y, u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_Showchar(unsigned int x, unsigned int y,unsigned char c, unsigned int color,unsigned int b_color);
void LCD_ShowString(unsigned int x, unsigned int y,const char *s, unsigned int color,unsigned int b_color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void LCD_ShowNum(unsigned int x, unsigned int y,int num, unsigned int color,unsigned int b_color);
void LCD_N(int x,int y,unsigned char num,unsigned int color,unsigned int b_color);
void TFT_FSMCConf(void);

#define LCD_SetPoint   LCD_DrawPoint




#endif

