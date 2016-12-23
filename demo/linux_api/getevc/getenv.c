/********************************************************************
* $ID: getenv.c             Thu 2016-10-20 11:18:08 +0800  lz       *
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

头文件：#include <stdlib.h>

定义函数：char * getenv(const char *name);

函数说明：getenv()用来取得参数name 环境变量的内容. 参数name 为环境变量的名称, 如果该变量存在则会返回指向该内容的指针. 环境变量的格式为name＝value.

返回值：执行成功则返回指向该内容的指针, 找不到符合的环境变量名称则返回NULL.

范例
#include <stdlib.h>
main()
{
   char *p;
   if((p = getenv("USER")))
   printf("USER = %s\n", p);
}

执行：
USER = root * */

const int prm_Default_fromENV(const char *env, char *obuf)
{
    char *ptr;

    ASSERT(env);
    ptr = getenv(env);

    if(!strncasecmp(env, "mac", 3)) {
    if(!ptr) ptr = "00:00:22:22:22:22";
    snprintf(obuf, 18+1, "%s", ptr);
    }
    else if(!strncasecmp(env, "hwver", 5)) {
    if(!ptr) ptr = "V1.01";
    snprintf(obuf, 9, "%s", ptr);
    }
    else if(!strncasecmp(env, "hwdate", 6)) {
    if(!ptr) ptr = "20121212";
    snprintf(obuf, 9, "%s", ptr);
    }
    else snprintf(obuf, 64, "%s", ptr);

    return 0;
}



/******************** End Of File: getenv.c ********************/
