
#ifndef __FATFS_DEMO_H__
#define  __FATFS_DEMO_H__

#include <stdint.h>
#include "ff.h"
#include "GlobalDef.h"





// 磁盘信息
typedef struct
{
      uint8_t mount; // 挂载是否成功; 0: 成功; 非0: 失败
      uint32_t total;  // 总容量, KB
      uint32_t free;   // 剩余容量, KB
}DiskInfo;

extern DiskInfo diskInfo[];

 extern long strtol(const char *str, char **endptr, int base);

FRESULT FILE_Open(FIL * pFileFIL,  char * path, char * file_name);
void FatFs_Demo(void);
void FILE_FormatDisk(FATFS * disk,  const char * diskPath,  uint8_t format);
void FILE_SearchUpdateBinFile(const char * diskPath);

uint16_t FILE_GetDiskFree(uint8_t disk_index);



#endif  //end of file

