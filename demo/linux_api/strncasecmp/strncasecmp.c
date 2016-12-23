/********************************************************************
* $ID: strncasecmp.c        Thu 2016-10-20 11:24:22 +0800  lz       *
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

/**
 *头文件：#include <string.h>

定义函数：int strncasecmp(const char *s1, const char *s2, size_t n);

函数说明：strncasecmp()用来比较参数s1 和s2 字符串前n个字符，比较时会自动忽略大小写的差异。

返回值：若参数s1 和s2 字符串相同则返回0。s1 若大于s2 则返回大于0 的值，s1 若小于s2 则返回小于0 的值。

范例
#include <string.h>
main(){
    char *a = "aBcDeF";
    char *b = "AbCdEf";
    if(!strncasecmp(a, b, 3))
    printf("%s =%s\n", a, b);
}

执行结果：
aBcDef=AbCdEf
 */
int prm_Default_fromENV(const char *env, char *obuf)
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


/****************** End Of File: strncasecmp.c ******************/
