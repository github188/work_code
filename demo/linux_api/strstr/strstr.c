/********************************************************************
* $ID: strstr.c             Wed 2016-09-28 15:58:02 +0800  lz       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (lizhu)  <lizhu@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2016 ZHYTEK                                         *
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
/*strstr（在一字符串中查找指定的字符串）
 *#include<string.h>
 *char *strstr(const char *haystack,const char *needle);
 *strstr()会从字符串haystack 中搜寻字符串needle，并将第一次出现的地址返回。
 *返回指定字符串第一次出现的地址，否则返回0。
 * */

int prm_SetBoard(ParamInfo *pinfo)
{
    char command[256];
//    char addr[32];
    int fd;//, gpioVal = 0;
    
    // check runing nfs
    if((fd = open("/proc/cmdline",  O_RDONLY)) > 0) {
    read(fd, command, 256);
    close(fd);
    
    if(strstr(command, "ttymxc0")) {
        sprintf(g_rs232_dev, "/dev/%s", "ttymxc1");
    }
    else
        sprintf(g_rs232_dev, "/dev/%s", "tty0");
    
    if(strstr(command, "nfsroot")) {
        printf("Run NFS\n");
        return 0;
    }
    }
}



/******************** End Of File: strstr.c ********************/
