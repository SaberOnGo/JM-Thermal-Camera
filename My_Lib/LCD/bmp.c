
#include "bmp.h"
#include "tft_commands.h"
#include "LCD.h"
#include "os_global.h"
#include "ff.h"
#include "fatfs_demo.h"
#include "CCM.h"
extern  FIL flashFIL;


u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0, g=0, b=0;
	   
	LCD_SetCursor(x,y);	    
	LLCD_WRITE_CMD(0X2E);  

       r = LCD_RD_DATA();
 	r = LCD_RD_DATA();    //实际坐标颜色
  
	b = LCD_RD_DATA(); 
	g = r & 0XFF;	
	g <<= 8;
	
       return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
}

static uint16_t bmp_cnt;

u8 bmp_encode(u16 x0, u16 y0, u16 x1, u16 y1)
{				
	u16 bmpheadsize;			// bmp头大小	   	
 	BITMAPINFO hbmp;			// bmp头	 
	FRESULT res;
       u32 y_idx, x_idx;
       u32  buf_idx = 0;
       u16 * bmp_buf;
       UINT bw = 0;
 	char tmp_name[20]; 
 	u16  width = x1 -x0 + 1;
 	u16 height = y1 - y0 + 1;
       u16 h = height;

       bmp_buf = (u16 *)ccm_buf;
	bmpheadsize = sizeof(hbmp);  // 得到bmp文件头的大小   
	os_memset((u8*)&hbmp,0,sizeof(hbmp)); // 置零空申请到的内存.	    
	
	hbmp.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); // 信息头大小
	hbmp.bmiHeader.biWidth   = width;	       // bmp的宽度
	hbmp.bmiHeader.biHeight = height; 	// bmp的高度
	hbmp.bmiHeader.biPlanes  = 1;	 		// 恒为1
	hbmp.bmiHeader.biBitCount = 16;	 	// bmp为16位色bmp
	hbmp.bmiHeader.biCompression = BI_BITFIELDS; // 每个象素的比特由指定的掩码决定。
 	hbmp.bmiHeader.biSizeImage = height  * width * (hbmp.bmiHeader.biBitCount / 8);  // bmp数据区大小
 				   
	hbmp.bmfHeader.bfType=((u16)'M'<<8)+'B';//BM格式标志
	hbmp.bmfHeader.bfSize      = bmpheadsize+hbmp.bmiHeader.biSizeImage;  // 整个bmp的大小
   	hbmp.bmfHeader.bfOffBits= bmpheadsize;//到数据区的偏移

	hbmp.RGB_MASK[0]=0X00F800;	 		//红色掩码
	hbmp.RGB_MASK[1]=0X0007E0;	 		//绿色掩码
	hbmp.RGB_MASK[2]=0X00001F;	 		//蓝色掩码

       os_snprintf((char*)tmp_name, sizeof(tmp_name), "%d.bmp",  bmp_cnt++);
       res = FILE_Open( &flashFIL,  "0:/image",  tmp_name);
       if(res){  os_printf("snap failed: res = %d\r\n",  res);  return 1;   }
       
	res=f_write(&flashFIL, (u8*)&hbmp, bmpheadsize, &bw); // 写入BMP首部  

       h = height;
       for(y_idx = y0 + h - 1;  h; y_idx--)
        //for(y_idx = y0; y_idx < h; y_idx++)
       {
	       for(x_idx = x0;  x_idx < width; x_idx++)
		{
			bmp_buf[buf_idx] = LCD_ReadPoint(x_idx, y_idx);
			buf_idx++;
			
                     if((buf_idx * 2) >= sizeof(ccm_buf))
                     {
                             os_printf("write bmp: buf_idx *2 = %ld, %ld \r\n",  buf_idx * 2,   os_get_tick());
                             res = f_write(&flashFIL,  bmp_buf,  buf_idx * 2,  &bw);
                             buf_idx = 0;
                     }
		}
		h--;
       }

       if(buf_idx)
       {
               os_printf("write last, write_cnt = %ld, %ld \r\n", buf_idx * 2,  os_get_tick());
               res = f_write(&flashFIL,  bmp_buf,  buf_idx * 2,  &bw);
       }
       res = f_close(&flashFIL); 

       
	return (u8)res;
}

