/********************************************************************
* $ID: isalpha.c            Wed 2016-09-28 15:45:01 +0800  lz       *
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
/*
 *isalpha （测试字符是否为英文字母）
 *		#include<ctype.h>
 *【参数】c 为需要被检测的字符。
 *【返回值】若参数c 为英文字母（a ~ z  A ~ Z），则返回非 0 值，否则返回 0。
 *【实例】找出str 字符串中为英文字母的字符。
 *
 *
 *
 * */

#include <stdio.h>
#include <ctype.h>

int main()
{
	 char str[] = "123c@#FDsP[e?";
	 int i;
	 for (i = 0; str[i] != 0; i++)
	 {
		if(isalpha(str[i]))
			printf("%c is an alphanumeric character\n", str[i]);
	 }
}


/******************** End Of File: isalpha.c ********************/
