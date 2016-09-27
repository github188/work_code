/********************************************************************
* $ID: misc.c               Thu 2016-09-22 15:46:03 +0800  lz       *
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
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "printdef.h"

struct tagCURTIMEVAL{
	unsigned int sec;
	unsigned int msec;
	unsigned int msec_cnt;
}systime;

unsigned int misc_update_time(void)
{
	struct timespec tp;

	clock_gettime(CLOCK_MONOTONIC, &tp);                 //基于Linux C语言的时间函数,他可以用于计算精度和纳秒

	systime.sec     = tp.tv_sec;
	systime.msec    = tp.tv_nsec / 1000000;
	systime.msec_cnt = (tp.tv_sec * 1000 + systime.msec);
	return tp.tv_sec;
}

unsigned int misc_current_time(int bUpdate)
{
	if(bUpdate) misc_update_time();
	return systime.msec_cnt;
}



/********************* End Of File: misc.c *********************/
