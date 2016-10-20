/********************************************************************
* $ID: open.c               Wed 2016-10-19 17:04:33 +0800  lz       *
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int agec, char *argv[])
{
	int fd, rdlen = 0;
	int offset = 3;
	char s[] = "Linux Programmer\n";
	char obuf[300];

	if((fd = open("1.txt", O_WRONLY|O_CREAT, NULL)) < 0) {
		printf("open fname error \n");
		return -1;
	}	
	write(fd,s,sizeof(s));
	close(fd);
	
	
	if((fd = open("1.txt", O_RDONLY, NULL)) < 0) {
		printf("open fname error \n");
		return -1;
	}	
	lseek(fd, offset, SEEK_SET);
	rdlen = read(fd, obuf, 5);

	printf("%s\n",obuf);
	
	close(fd);

	return 0;
}

/*
 * */

/********************* End Of File: open.c *********************/
