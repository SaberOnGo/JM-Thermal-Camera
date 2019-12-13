
#include "tft_core.h"
#include "tft_commands.h"
#include "LCD.h"


void LCD_setAddressWindow(u16 x0, u16 y0, u16 x1, u16 y1) {
       LLCD_WRITE_CMD(LCD_COLUMN_ADDR);   
	LLCD_WRITE_DATA(x0 >> 8);
	LLCD_WRITE_DATA(x0 & 0xff);
	LLCD_WRITE_DATA(x1 >> 8);
	LLCD_WRITE_DATA(x1 & 0xff);

	LLCD_WRITE_CMD(LCD_PAGE_ADDR);   
	LLCD_WRITE_DATA(y0 >> 8);
	LLCD_WRITE_DATA(y0 & 0xff);
	LLCD_WRITE_DATA(y1 >> 8);
	LLCD_WRITE_DATA(y1 & 0xff);
}


void LCD_Scan_Dir(E_LCD_SCAN_DIR dir)
{
       u16 regval=0;

	switch(dir)
	{
		case L2R_U2D://从左到右,从上到下
			regval |=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval |=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval |=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval |=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval |=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval |=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval |=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
			regval |=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	LCD_WriteReg(0x36, regval);
}

#include "ff.h"
#include "fatfs_demo.h"
#include "CCM.h"

#define  GETR_RGB16(RGB565)                          ((u8)((RGB565 >> 8) & 0xF8))
#define  GETG_RGB16(RGB565)                          ( (u8)((RGB565 >>3) & 0xFC))
#define  GETB_RGB16(RGB565)                           ((u8)((RGB565 << 3) & 0xF8))


static u8 bmp_header[54]={//构造bmp首部
      #if 0
	0x42, 0x4D, 0x36, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	#else
       0x42, 0x4d, 0, 0,   // 0 -3
		0, 0, 0, 0,   // 4 - 7
		0, 0, 54, 0, // 8 - 11
		0, 0, 40,0,  // 12 - 15
		0, 0, 0, 0,   // 16 - 19
		1, 0, 24, 0,  // 20 - 23
		0, 0, 0, 0,    // 24 - 27
		0, 0, 0, 0,    // 28 - 21
		0, 0, 0, 0,    // 22 - 25
		0, 0, 0, 0,    // 26 - 29
		0, 0, 0, 0,     // 30 - 33
		0, 0, 0, 0      // 34 - 37
	#endif
};



extern  FIL flashFIL;

#define LCD_setAddressWindowToRead(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    LLCD_WRITE_CMD(LCD_RAMRD)

#define LCD_setAddressWindowToWrite(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    LLCD_WRITE_CMD(LCD_GRAM)
    

#define FILL_BMP_HEADER(bmp_header,  file_size, width, height) \
       bmp_header[2] =  (u8)file_size;\
       bmp_header[3] = (u8)(file_size >> 8);\
       bmp_header[4] = (u8)(file_size >> 16);\
       bmp_header[5] = (u8)(file_size >> 24);\
       \
       bmp_header[18] = (u8)width;\
	bmp_header[19] = (u8)(width >> 8);\
	bmp_header[20] = (u8)(width >> 16);\
	bmp_header[21] = (u8)(width >> 24);\
	\
	bmp_header[22] = (u8)height;\
	bmp_header[23] = (u8)(height >> 8);\
	bmp_header[24] = (u8)(height >> 16);\
	bmp_header[25] = (u8)(height >> 24);


u8 DiskHasEnoughFreeSpace(void)
{
        uint32_t free;
        free = FILE_GetDiskFree(FLASH_DISK);
        if(free > (512))  // 剩余空间超过 512 KB
        {
               os_printf("disk free space: %ld KB \r\n",  free);
               return 1;
        }
	 else
	 {
	          os_printf("disk space is not enough: %ld KB \r\n", free);
                 return 0;
	 }
}

FRESULT TFT_GenerateBmp(FIL* fp, u16 x0, u16 y0, u16 x1,  u16 y1, u8 * buf, uint32_t buf_size)
{
        FRESULT res;

      // u16  R = 0, G = 0, B = 0;
        u16 c0,c1, c2;
        u8 R0,G0,B0;
        u8 R1,G1,B1;
        
 	 //u16 color;
        uint32_t idx;
        u32 buf_idx  = 0;
    	 u32 pixel_count;
    	 UINT bw = 0;
    	 
        LCD_setAddressWindowToRead(x0, y0, x1, y1);

        pixel_count = (u32) ((x1 - x0 + 1) * (y1 - y0 + 1));
        pixel_count /= 2;
        
        c0 = LLCD_READ_DATA(); // dummy data
        for (idx = 0; idx < pixel_count; ++idx) 
       {
                c0     = LLCD_READ_DATA();
                c1     = LLCD_READ_DATA();
                c2     = LLCD_READ_DATA();
                
                R0  = (u8)(c0 >> 11);
                G0  = (u8)(c0 >> 2);
                
                B0   = (u8)(c1 >> 11);
                R1   = (u8)(c1 >> 3);

                G1   = (u8)(c2 >> 10);
                B1   = (u8)(c2 >> 3);
                
                //color = (((R>>11) <<11) | ((G>>10)<<5) | (B >> 11));
 
                buf[buf_idx * 6 + 0]  =  B0;  // B
                buf[buf_idx * 6 + 1]  =  G0;  // G
                buf[buf_idx * 6 + 2]  =  R0;  // R
                buf[buf_idx * 6 + 3]  = B1;
                buf[buf_idx * 6 + 4]  = G1;
                buf[buf_idx * 6 + 5]  = R1;
                
                 buf_idx++;
                 if((buf_idx * 6) >=  buf_size)
                 {
                        os_printf("write bmp: buf_idx *6 = %ld, %d, %ld \r\n", 
                                                 buf_idx * 6,  buf_size, os_get_tick());
                        f_write(fp,  buf,  buf_size,  &bw);
                         buf_idx = 0;
                 }
          }
          if(buf_idx)
	   {
                     os_printf("write last, write_cnt = %ld, %ld \r\n", buf_idx,  os_get_tick());
                     res = f_write(fp,  buf,  buf_idx * 6,  &bw);
	    }
	    res = f_close(fp); 

          return res;
}



static uint16_t img_cnt = 0;
int Screen_shot(u16 x0, u16 y0, u16 x1,  u16 y1)
{
	uint32_t file_size;     
	char tmp_name[20];
	
	 u16 width, height;
	 FRESULT res;
        UINT bw = 0;
        
        if(! DiskHasEnoughFreeSpace()) return -1;
        
	 width   = x1 - x0 + 1;
	 height = y1 - y0 + 1;
	 
	/* 宽*高 +补充的字节 + 头部信息 */
       file_size = ((uint32_t)height) * ALIGN4_SIZE(width) * 3 + 54;
       
	FILL_BMP_HEADER(bmp_header, file_size, width, height); // construct bmp header
	
	os_snprintf((char*)tmp_name, sizeof(tmp_name), "%d.bmp",  img_cnt++);
       res = FILE_Open( &flashFIL,  "0:/image",  tmp_name);
       if(res){  os_printf("snap failed: res = %d\r\n",  res);  return -1;   }
       
       res = f_write(&flashFIL, bmp_header, 54, &bw);	  // write bmp header	 
       res = TFT_GenerateBmp(&flashFIL, x0, y0, x1, y1, ccm_buf, sizeof(ccm_buf));

       return 0;
}

void LCD_putPixel(u16 x, u16 y, u16 color) {
    LCD_setAddressWindowToWrite(x, y, x, y);
    LLCD_WRITE_CMD(LCD_GRAM);
    LLCD_WRITE_DATA(color); 
}

void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color) {
    LCD_Fill(x1, y1, x1 + w -1, y1 + h - 1, color);
}

void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color) {
    if (h == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    LCD_fillRect(x0, y0, 1, h, color);
}

void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color) {
    if (w == 1) {
        LCD_putPixel(x0, y0, color);
        return;
    }
    LCD_fillRect(x0, y0, w, 1, color);
}

void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color) {
    if (w == 0 || h == 0) return;
    if (w == 1) {
        LCD_drawFastVLine(x, y, h, color);
        return;
    }
    if (h == 1) {
        LCD_drawFastHLine(x, y, w, color);
        return;
    }
    LCD_drawFastHLine(x, y, w, color);
    LCD_drawFastHLine(x, (u16) (y + h - 1), w, color);
    LCD_drawFastVLine(x, y, h, color);
    LCD_drawFastVLine((u16) (x + w - 1), y, h, color);
}

/*
void DispPowerShape(void)
{
		LCD_DrawLine(181,6,181,25,WHITE);
		LCD_DrawLine(216,6,216,25,WHITE);
		LCD_DrawLine(181,6,216,6,WHITE);
		LCD_DrawLine(181,25,216,25,WHITE);
		LCD_DrawLine(180,11,180,20,WHITE);
		LCD_DrawLine(179,11,179,20,WHITE);
		LCD_DrawLine(178,11,178,20,WHITE);
}*/

//
void DispSnapTip(u16 color, u8 cross)
{
	 LCD_drawRect(150, 6, 20, 20, color);
	 LCD_drawRect(151, 7, 18,18, color);
	 if(cross)
	 {
              LCD_DrawLine(150, 6, 170, 25, color);
              LCD_DrawLine(170, 6, 150, 25, color);
	 }
}



extern u8 bmp_encode(u16 x0, u16 y0, u16 x1, u16 y1);

void LCD_Snapshot(void)
{

         VBUS_Ctrl_Close();

         if(DiskHasEnoughFreeSpace())  // has enough disk space to write
         {
               DispSnapTip(WHITE, 0);   // tip generate bmp image 
               bmp_encode(0, 0,  X_MAX_PIXEL - 1,  Y_MAX_PIXEL -1);
               DispSnapTip(BLACK, 0);
         }
         else
         {
                // tip generate bmp image failed
                DispSnapTip(RED, 1);
                delay_ms(800);
                DispSnapTip(BLACK, 1);
         }
         LCD_setAddressWindowToWrite(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL -1);

}

