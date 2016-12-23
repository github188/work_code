/*************************************************************************
    > File Name: main.c
    > Author: lizhu
    > Mail: 1489306911@qq.com 
    > Created Time: 2016年07月07日 星期四 09时42分56秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>
int main(){
	char s[50]={'0'};
	int i ,n=0,m=0;
	gets(s);
	for(i=0;i<strlen(s);i++){
		if(s[i]>='a' && s[i]<='z') n++;
		if(s[i]>='A' && s[i]<='Z') m++;
	}
	printf("\nx小写出现的次数为%d次  大写出现的次数为%d次",n,m);
}
