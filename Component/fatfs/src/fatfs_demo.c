

 
#include "GlobalDef.h"
#include "fatfs_demo.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "os_global.h"
#include "delay.h"
 


#if FAT_DEBUG_EN
#define fat_printf(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define fat_printf(...)
#endif


/* Private variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if FLASH_DISK_EN
#include "spi_flash_interface.h"
FATFS FlashDiskFatFs;         /* Work area (file system object) for logical drive */
const char    FlashDiskPath[3] = "0:";
const char   UpdateDir[16] = "update";   // 升级文件父目录
 char FlashFilePath[64];
 FIL flashFIL;
 
#endif


#if SD_DISK_EN
#include "sd_card_interface.h"
FATFS SDDiskFatFs;  // SD Card
const char    SDDiskPath[3] = "1:";
#endif


#if ROM_DISK_EN
#include "rom_flash_interface.h"
FATFS ROMDiskFatFs;  // ROM FLASH
const char    ROMDiskPath[3] = "2:";
FIL  sdFIL;
#endif
 


#if _USE_LFN
char Lfname[_MAX_LFN + 1];
#endif



//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
FRESULT exf_getfree(uint8_t * drv, uint32_t * total, uint32_t *free)
{
	FATFS * fs1;
	FRESULT res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;

     *free = 0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res == FR_OK)
	{											   
	           tot_sect = (fs1->n_fatent - 2) * fs1->csize;	//得到总扇区数
	          fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
 
        #if (_MAX_SS == 4096L)
		*total = tot_sect  << 2;	// x4, 单位为KB, 扇区大小为 4096 B
		*free  = fre_sect  << 2;	// x4, 单位为KB 
		#elif (_MAX_SS == 512L)
        *total = tot_sect  / 2;	//单位为KB, 这里的FatFs扇区大小为 512 B
		*free  = fre_sect  / 2;	//单位为KB 
        #else
		#error "ext_getfree error"
		#endif
 	}
	else
	{
	    fat_printf("FatFs error = %d\n", res);
	}
	return res;
}	


 

/*
功能: 在字符流中查找匹配条件的字符并将其转换为数字
参数: char * buf: 待查找的字符串
             char * str: 匹配的子字符串
             uint16_t strLen: 子字符串的长度
             uint16_t min: 转换为数字, 数字的最小值
             uint16_t max: 转换为数字后的最大值
             char **p: 符合匹配条件的字符串的起始指针的指针
             uint16_t * num: 成功转换为数字的指针
返回值: 转换成功返回 APP_SUCCESS; 失败返回 APP_FAILED
*/
E_RESULT StringToInt(char * buf, char * str, uint16_t strLen, 
                                                         uint16_t min, uint16_t max, char ** p, uint16_t * num)
{
     char * p1 = NULL;
	 char * pEnd = NULL;
	 uint16_t len = 0;
	 uint16_t n = 0;

	 *num = 0;
     p1 = os_strstr((const char *)buf, str);  // 查找匹配的子字符串
     if(p1)
     {
        p1 += strLen;
        len = os_strlen(p1);
		if(len)
		{
		    n = strtol(p1, &pEnd, 10);
			if(0 == n && pEnd == (char *)p1)
			{
			   fat_printf("err:%s, %d, n=%d, 0x%x, 0x%x\n", __FILE__, __LINE__, n, (uint32_t)pEnd, (uint32_t)p1);
			   goto Exit;
			}
			if(min <= n && n <= max)
			{
			   *p   = p1;
			   *num = n;
			   fat_printf("n = %d\n", n);
			   return APP_SUCCESS;
			}
			else { goto Exit; }
		}
     }
	 else { fat_printf("p1 null: %s, %d\n", __FILE__, __LINE__); }
	 
Exit:
	*p = NULL;
	return APP_FAILED;
}

/*
功能: 在字符流中查找匹配条件的字符串
参数: char * buf: 待查找的字符串
             char * str: 匹配的子字符串
             char **p: 符合匹配条件的字符串的起始指针的指针
             uint16_t * matchStrLen: 目的字符串长度
返回值: 转换成功返回 APP_SUCCESS; 失败返回 APP_FAILED
*/
E_RESULT StringToString(char * buf, char * str, char ** p, uint16_t * matchStrLen)
{
        char * p1 = NULL;
	 char * pEnd = NULL;
	 uint16_t len = 0;

	 *matchStrLen = 0;
        p1 = os_strstr((const char *)buf, str);  // 查找匹配的子字符串
        if(p1)
       {
              p1 += os_strlen(str);
              len = os_strlen(p1);
	       if(len)
		{
		       pEnd = os_strchr(p1, '\"');
		       *matchStrLen = pEnd - p1;
		       *p = p1;
		       return APP_SUCCESS;
		}
        }
	 else 
	 { fat_printf("p1 null: %s, %d\n", __FILE__, __LINE__); }
	 
//Exit:
	*p = NULL;
	return APP_FAILED;
}

 // 打开指定文件
FRESULT FILE_Open(FIL * pFileFIL,  char * path, char * file_name)
{
   FRESULT res;
   char  file_path[64];        // 传感器的目录
   DIR dirs;

   // 判断目录是否存在, 不存在则创建
   res = f_opendir(&dirs, (const TCHAR *)path);  // 打开所在目录
   fat_printf("open dir %s %s\n",  path, (res == FR_OK ? "ok" : "failed"));
   if(res == FR_NO_PATH)  
   {
            // 创建该目录
          res = f_mkdir((const TCHAR *)path);
          
	   if(res != FR_OK){ fat_printf("create dir %s failed, %d\n", path, res); return res; }
	   else { fat_printf("create dir %s success\n", path); }
	  
   }
   else if(res){  fat_printf("file err: %s %d: res = %d \r\n",  __FILE__, __LINE__,  res);  return res; }
   //f_closedir(&dirs);
   
   // 构建文件路径
   os_snprintf(file_path,  sizeof(file_path), "%s/%s", path, file_name);

   res = f_open(pFileFIL, (const TCHAR * )file_path, FA_OPEN_ALWAYS | FA_WRITE );  // 创建文件
   fat_printf("f_open: res = %d \r\n",  res);
   if(res)
   {
         f_closedir(&dirs);
         f_close(pFileFIL);
   }
    return res;
}


#define RECURSIVE_EN   0  // 递归搜索使能: 1; 禁止: 0

FRESULT FILE_Scan(
	char * path,		   /* Pointer to the working buffer with start path */
	int   pathMaxLen,    /* the max length of the working buffer  */
	char * fileInName,   /*   待查找的文件名 */
	char * filePath,     // 如果查找成功, 则拷贝文件路径到此
	int filePathMaxLen // filePath buf 的最大长度
)
{
	FILINFO  Finfo;
	DIR dirs;
	FRESULT res;
    char *fn;
    WORD AccFiles = 0;  /* AccFiles 个file 及AccDirs 个folders */
	DWORD AccSize = 0;				/* 文件中的字节数 */
	char * p = NULL;
      char * postfix = NULL;
      char * pfix = NULL;
	
#if FAT_DEBUG_EN
	FRESULT result;
#endif
	
#if RECURSIVE_EN
    WORD AccDirs = 0;		
    int len;                
#endif
	
#if _USE_LFN
	Finfo.lfname = Lfname;
	Finfo.lfsize = sizeof(Lfname);
#endif
	res = f_opendir(&dirs, path);
	if (res == FR_OK) 
	{
	    #if RECURSIVE_EN
		len = os_strlen(path);  // 不需要递归搜索
		#endif
		
		while ((f_readdir(&dirs, &Finfo) == FR_OK) && Finfo.fname[0]) 
		{
			if (_FS_RPATH && Finfo.fname[0] == '.') 
				continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR)   // 是目录
			{
			    #if RECURSIVE_EN // 如需递归搜索打开这部分代码 , 并且FileOutPath 为外部静态数组
				AccDirs++;
				path[len] = '/'; 
				os_strncpy(path + len + 1, fn, pathMaxLen);
				//记录文件目录

				res = FILE_Scan(path, fileInName, FlashFilePath);
				path[len] = '\0';
				if (res != FR_OK)
				{
				   fat_printf("scan file() error  = %d\n", res);
				   break;
				}	
				#else
				
				fat_printf("scan file failed, dirs\n");
				res = FR_NO_FILE;
				break;
				
				#endif
			} 
			else   // 是文件
			{
				AccFiles++;
				AccSize += Finfo.fsize;

				//记录文件
				os_snprintf(filePath, filePathMaxLen, "%s/%s\r\n", path, fn);  // 文件路径最长为256 B
				p = os_strstr(filePath, fileInName);
				postfix = os_strstr(filePath, "bin");
				pfix = os_strstr(filePath, "2to1");   // 不含boot文件
				if (p && postfix && (!pfix))
				{
				    
					fat_printf("search file ok: %s\n", filePath);
					res = FR_OK;
				}
				else
				{
				    fat_printf("not find file\n");
				    res = FR_NO_FILE;
				}
			}
		}
	}
#if FAT_DEBUG_EN
	result = 
#endif
    f_closedir(&dirs);
	fat_printf("closedir %s\n", (result == FR_OK ? "OK" : "Failed"));
	
	return res;
}




#define DISK_IDX(diskPath)    (diskPath[0] - 0x30)

DiskInfo diskInfo[_VOLUMES];
void FILE_FormatDisk(FATFS * disk,  const char * diskPath,  uint8_t format)
{
	FRESULT res;
//#if FAT_DEBUG_EN
    uint32_t total = 0, free = 0;
//#endif

	// 挂载磁盘
	res = f_mount(disk, diskPath, 1);
	diskInfo[DISK_IDX(diskPath)].mount = res;

	if (res == FR_NO_FILESYSTEM)  // FAT文件系统错误,重新格式化FLASH
	{
	       if(format)
	       {
       	       fat_printf("f_mout: %s error, do format...\r\n",  diskPath);
       		res = f_mkfs((TCHAR const*)diskPath, 1, 4096);  //格式化FLASH,diskPath: 盘符; 1,不需要引导区,8个扇区为1个簇
       		if(res == FR_OK)
       		{
       		       fat_printf("%s Disk Format Finish !\n", diskPath);  
       		}
       		else
       		{
       		      fat_printf("%s Disk Format Failed = %d\n",  diskPath, res);
       		}
		}
	}
	else
	{
	        if(res  != FR_OK)
	       {
	              fat_printf("f_mount: %s = %d\r\n",  diskPath, res);
	       }
		else
		{
		       fat_printf("f_mount: %s success\r\n", diskPath);
			//FILE_SearchUpdateBinFile(diskPath); // 搜索bin文件, 符合条件则升级
			
			if(exf_getfree((uint8_t *)diskPath, &total, &free) == FR_OK)
		      {
		             fat_printf("%s: read fatfs file size success\r\n", diskPath);
			      fat_printf("total = %ld KB, free = %ld KB\n", total, free);
			      diskInfo[DISK_IDX(diskPath)].free  = free;
			      diskInfo[DISK_IDX(diskPath)].total = total;
		      }
		}
	}	
}

// 获取磁盘空闲大小
// 参数: 0 -N,  0: 表示磁盘0
//返回值: 磁盘空闲大小: 单位: KB
uint16_t FILE_GetDiskFree(uint8_t disk_index)
{
        uint32_t total = 0, free = 0;
        uint8_t diskPath[3] = {0, 0, 0};

        os_snprintf(diskPath,  3,  "%d:", disk_index);
        if(exf_getfree((uint8_t *)"0:", &total, &free) == FR_OK){}
        return free;
}


void FatFs_Demo(void)
{

#if FLASH_DISK_EN
       FILE_FormatDisk(&FlashDiskFatFs,  FlashDiskPath,  0);
#endif

#if SD_DISK_EN
       FILE_FormatDisk(&SDDiskFatFs,        SDDiskPath,        0);
#endif

#if ROM_DISK_EN
       FILE_FormatDisk(&ROMDiskFatFs,        ROMDiskPath,        1);
#endif

}


