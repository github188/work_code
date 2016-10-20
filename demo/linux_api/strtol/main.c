/********************************************************************
* $ID: main.c               Wed 2016-09-28 10:37:35 +0800  lz       *
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
/*
 strtol() 函数用来将字符串转换为长整型数(long)，其原型为：
 long int strtol (const char* str, char** endptr, int base);*
 *
 * */


// 192.168.1.22 -->C0 A8 01 16
int prm_netip_2_binary(unsigned char *dst, const char *src)
{
    int i, cnt = 4;; 
    char *ptr = (char*)src;

    if(!dst || !src) return -1; 

    for(i = 0; i < cnt; i++) {
    dst[i] = (unsigned char)strtol(ptr, (char**)NULL, 10);

    if((ptr = index(ptr, '.'))) ptr += 1;
    else break;
    }   

    return (i >= cnt) ? 0 : -2; 
}

// aa:bb:cc:dd:dd:ee  -->AA BB CC DD DD EE
int prm_netmac_2_binary(unsigned char *dst, const char *src)
{
    int i, cnt = 6;; 
    char *ptr = (char*)src;

    if(!dst || !src) return -1; 

    for(i = 0; i < cnt; i++) {
    dst[i] = (unsigned char)strtol(ptr, (char**)NULL, 16);

    if((ptr = index(ptr, ':'))) ptr += 1;
    else break;
    }   

    return (i >= cnt) ? 0 : -2; 
}


int main()
{
	int i;
	char *date = "192.168.1.22";
	unsigned char dst[6];
	memset(dst,0,6);

	prm_netip_2_binary(dst,date);

	for(i = 0 ; i< 6 ; i++)
	{
		printf("[%x] ",dst[i]);
	}
	printf("\n");
}

/********************* End Of File: main.c *********************/
