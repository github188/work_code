/********************************************************************
* $ID: fprintf.c            Wed 2016-10-19 11:55:07 +0800  lz       *
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
//stderr 定向到屏幕上的错误打印
//stdout 输出到磁盘文件

#include <stdio.h>
int main()
{
	int fd = 1;
	fprintf(stderr,"WARNING! fd:%d RTSP connection truncated before ending operations.\n",fd);
	return 0;
}



/******************** End Of File: fprintf.c ********************/
