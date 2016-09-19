/********************************************************************
* $ID: param.c              Mon 2016-09-19 15:43:44 +0800  lz       *
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "public.h"
#include "printdef.h"


extern char g_rs232_dev[];
int prm_SetBoard(ParamInfo *pinfo)
{
	char command[256];
	int fd;

	if((fd = open("/proc/cmdline", O_RDONLY)) > 0)
	{
		read(fd, command, 256);
		close(fd);

		if(strstr(command, "ttymxc0")) //strstr（在一字符串中查找指定的字符串）
		{
			sprintf(g_rs232_dev, "/dev/%s", "ttymxc1");
		}
		else
			sprintf(g_rs232_dev, "/dev/%s", "tty0");

		if(strstr(command, "nfsroot"))
		{
			printf("RUN NFS\n");
			return 0;
		}
	}

	return 0;
}

int prm_Default(ParamInfo *pinfo)
{
	if(!pinfo) return -1;

	memset(pinfo , 0 , sizeof(ParamInfo));

	pinfo->listport = DEF_PORT;
	snprintf(pinfo->net_ip, sizeof(pinfo->net_ip), "%s", DEF_IP); //http://blog.csdn.net/crfoxzl/article/details/2062139
	snprintf(pinfo->net_mac, sizeof(pinfo->net_mac), "%s", "00:01:02:03:04:05");
	snprintf(pinfo->sn, sizeof(pinfo->sn), "%s", SER_NUM);
	return 0;
}


/********************* End Of File: param.c *********************/
