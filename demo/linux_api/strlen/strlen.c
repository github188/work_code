/********************************************************************
* $ID: main.c               Wed 2016-10-19 09:39:02 +0800  lz       *
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
// unsigned int strlen (char *s);
// strlen()用来计算指定的字符串s 的长度，不包括结束字符"\0"。
//   注意一下字符数组，例如
//       char str[100] = "http://see.xidian.edu.cn/cpp/u/biaozhunku/";
//       定义了一个大小为100的字符数组，但是仅有开始的11个字符被初始化了，剩下的都是0，所以 sizeof(str) 等于100，strlen(str) 等于11。
//如果字符的个数等于字符数组的大小，那么strlen()的返回值就无法确定了，例如
//    char str[6] = "abcxyz";
//    strlen(str)的返回值将是不确定的。因为str的结尾不是0，strlen()会继续向后检索，直到遇到'\0'，而这些区域的内容是不确定的。
//
//    注意：strlen() 函数计算的是字符串的实际长度，遇到第一个'\0'结束。如果你只定义没有给它赋初值，这个结果是不定的，它会从首地址一直找下去，直到遇到'\0'停止。而sizeof返回的是变量声明后所占的内存数，不是实际长度，此外sizeof不是函数，仅仅是一个操作符，strlen()是函数。
//    #include<stdio.h>
//    #include<string.h>
//    int main()
//    {
//        char *str1 = "http://see.xidian.edu.cn/cpp/u/shipin/";
//        char str2[100] = "http://see.xidian.edu.cn/cpp/u/shipin_liming/";
//        char str3[5] = "12345";
//        printf("strlen(str1)=%d, sizeof(str1)=%d\n", strlen(str1), sizeof(str1));
//        printf("strlen(str2)=%d, sizeof(str2)=%d\n", strlen(str2), sizeof(str2));
//        printf("strlen(str3)=%d, sizeof(str3)=%d\n", strlen(str3), sizeof(str3));
//        return 0;
//    }
//

#include <stdio.h>
#include <string.h>
int main() {
	char str[100]; 
	int i;
	char cc;
	printf("please input str:");
	scanf("%s",str);
	printf("please input cc:");
	getchar();
	scanf("%c",&cc);
	for(i = 0; i < strlen(str); i++) {
		if(str[i] == cc) {
			printf("%d\n", i);
		}
	}

	return 0;
}
/********************* End Of File: main.c *********************/
