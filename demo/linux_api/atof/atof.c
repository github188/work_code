/********************************************************************
* $ID: atof.c               Wed 2016-09-28 15:55:50 +0800  lz       *
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
 *头文件：#include <stdlib.h>
 *函数 atof() 用于将字符串转换为双精度浮点数(double)，其原型为：double atof (const char* str);
 *【返回值】返回转换后的浮点数；如果字符串 str 不能被转换为 double，那么返回 0.0。
 *
 *
 * */

#include <stdio.h>
#include <stdlib.h>
int main(){
    char *a = "-100.23",
         *b = "200e-2",
         *c = "341",
         *d = "100.34cyuyan",
         *e = "cyuyan";
    printf("a = %.2f\n", atof(a));
    printf("b = %.2f\n", atof(b));
    printf("c = %.2f\n", atof(c));
    printf("d = %.2f\n", atof(d));
    printf("e = %.2f\n", atof(e));
    system("pause");
    return 0;
}


/********************* End Of File: atof.c *********************/
