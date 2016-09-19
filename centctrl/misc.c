/********************************************************************
* $ID: misc.c        Thu 2012-11-08 13:55:12 +0800  dannywang       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <dannywang@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2012 ZHYTEK                                      *
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

struct tagCURTIMEVAL {
    unsigned int   sec;
    unsigned int   msec;
    unsigned int   msec_cnt;
}systime;

unsigned int misc_update_time(void)
{
    struct timespec tp;

    clock_gettime(CLOCK_MONOTONIC, &tp);

    systime.sec      = tp.tv_sec;
    systime.msec     = tp.tv_nsec / 1000000;
    systime.msec_cnt = (tp.tv_sec * 1000 + systime.msec);
    return tp.tv_sec;
}

unsigned int misc_current_time(int bUpdate)
{
    if(bUpdate) misc_update_time();

    return systime.msec_cnt;
}


// Read GPIO
// return :[BIT[0...7] 8 gpio status
unsigned int misc_read_serialGPIO(void)
{
    int i, fd, value;
    int status = 0;
    char fname[64];
    static unsigned char gpios[] = {128, 129, 130, 131, 135, 134, 133, 132};

    for(i = 0; i < 8; i++) {
	sprintf(fname, "/sys/class/gpio/gpio%d/value", gpios[i]);
	fd = open(fname, O_RDONLY);
	if(fd > 0) {
	    read(fd, fname, 2);
	    fname[1] = 0;

	    value = atoi(fname) ? 0 : 1;   //  设置为ON这位来表示1

	    status |= (value << i);
	    close(fd);
	}
    }

    return status;
}




/********************* End Of File: misc.c *********************/
