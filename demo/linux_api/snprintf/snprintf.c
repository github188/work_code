/********************************************************************
* $ID: snprintf.c           Thu 2016-10-20 10:01:08 +0800  lz       *
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
int main()
{
    char str[5];
    int ret = snprintf(str, 3, "%s", "abcdefg");
    printf("%d\n",ret);
    printf("%s",str);
    return 0;
}

//========================================
#if 1
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

// set all param default
int prm_Default(ParamInfo *pinfo)
{
    if(!pinfo) return -1; 

    memset(pinfo, 0, sizeof(ParamInfo));

    pinfo->listport  = DEF_PORT;
    snprintf(pinfo->net_ip, sizeof(pinfo->net_ip), "%s", DEF_IP);
    snprintf(pinfo->net_mac, sizeof(pinfo->net_mac), "%s", "00:0c:e9:b5:9a:06");
    snprintf(pinfo->sn, sizeof(pinfo->sn), "%s", SER_NUM);
    return 0;
}

#endif


/*
snprintf()函数用于将格式化的数据写入字符串，其原型为：
    int snprintf(char *str, int n, char * format [, argument, ...]);

【参数】str为要写入的字符串；n为要写入的字符的最大数目，超过n会被截断；format为格式化字符串，与printf()函数相同；argument为变量。

【返回值】成功则返回参数str 字符串长度，失败则返回-1，错误原因存于errno 中。

snprintf()可以认为是sprintf()的升级版，比sprintf()多了一个参数，能够控制要写入的字符串的长度，更加安全，只要稍加留意，不会造成缓冲区的溢出。

snprintf()与sprintf()使用方法类似，请参考：C语言sprintf()函数

注意：snprintf()并不是标C中规定的函数，但是在许多编译器中，厂商提供了其实现的版本。

在GCC中，该函数名称就snprintf()，而在VC中称为_snprintf()。由于不是标准函数，没有一个统一的标准来规定该函数的行为，所以导致了各厂商间的实现版本可能会有差异。

差异发生在参数 n。在GCC中，参数n是要向str写入3个字符，包括'\0'字符；在VC中，参数n是要写入的字符串的总字符数。
*/
/******************* End Of File: snprintf.c *******************/
