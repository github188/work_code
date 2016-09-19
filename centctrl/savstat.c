/********************************************************************
* $ID: savstat.c     Thu 2013-10-17 20:44:44 +0800  dannywang       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <dannywang@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2013 ZHYTEK                                         *
*   All rights reserved.                                            *
*                                                                   *
* This file is free software;                                       *
*   you are free to modify and/or redistribute it                   *
*   under the terms of the GNU General Public Licence (GPL).        *
*                                                                   *
* Last modified:                                                    *
*                                                                   *
* No warranty, no liability, use this at your own risk!             *
********************************************************************/
#include "public.h"
#include "pthread.h"

#include "printdef.h"

#define SAVSTAT_MAGIC          (0x1234ABCD)
#define SAVSTAT_HEAD_SIZE      (16)

#ifdef SAVE_SATAT_SPIFLASH
#define SAVSTAT_DEVNAME       "/dev/mtd5"
#define SAVSTAT_DEVOFFSET     0x500000     // 5M offset
#define SAVSTAT_DEVPAGE       0x20000      // 128K
extern int flashcp_memory_read(const char *devicename, int devoffset,
	char *bufFlag, int bufFlagsize, char *inBuf, int iSize);
extern int flashcp_memory_write(const char *devicename, int devoffset,
	const char *bufFlag, int bufFlagsize, const char *inBuf, int iSize);

#else

// 使用铁电存储器来保存当前的通道数据
// 存储器大小为 32K 每次读写大小为4K
#define SAVSTAT_DEVNAME      "/sys/bus/spi/devices/spi1.1/fram"
#define SAVSTAT_DEVOFFSET 0
#define SAVSTAT_DEVPAGE   0x4000 //每个参数区最大空间
static int flashcp_memory_write(const char *devicename, int devoffset,
	const char *bufFlag, int bufFlagsize, const char *inBuf, int iSize)
{
    int value, writed = 0, fd = -1;

    if((fd = open(devicename, O_RDWR)) < 0) {
	PERROR("Open SPI fram ERROR:%s\n", devicename);
	return -1;
    }

    lseek(fd, devoffset, SEEK_SET);
    if((value=write(fd, bufFlag, bufFlagsize)) != bufFlagsize) {
	PERROR("Read bufFlag status Error:value=%d!=%d\n", value, bufFlagsize);
	close(fd);
	return -2;
    }

    while(writed < iSize) {
	value = iSize - writed;
	if(value > 4096) value = 4096;

	if(write(fd, &inBuf[writed], value) < value) {
	    break; // read Error
	}

	writed += value;
    }
    if(writed < iSize) {
	PERROR("Read status Error:value=%d!=%d\n", writed, iSize);
	close(fd);
	return -2;
    }

    close(fd);
    return 0;
}

static int flashcp_memory_read(const char *devicename, int devoffset,
	char *bufFlag, int bufFlagsize, char *inBuf, int iSize)
{
    int value, readed = 0, fd = -1;

    if((fd = open(devicename, O_RDWR)) < 0) {
	PERROR("Open SPI fram ERROR:%s\n", devicename);
	return -1;
    }

    lseek(fd, devoffset, SEEK_SET);
    if((value=read(fd, bufFlag, bufFlagsize)) != bufFlagsize) {
	PERROR("Read bufFlag status Error:value=%d!=%d\n", value, bufFlagsize);
	close(fd);
	return -2;
    }

    while(readed < iSize) {
	value = iSize - readed;
	if(value > 4096) value = 4096;

	if(read(fd, &inBuf[readed], value) < value) {
	    break; // read Error
	}

	readed += value;
    }
    if(readed < iSize) {
	PERROR("Read status Error:value=%d!=%d\n", readed, iSize);
	close(fd);
	return -2;
    }

    close(fd);
    return 0;
}
#endif


/******************** End Of File: savstat.c ********************/
