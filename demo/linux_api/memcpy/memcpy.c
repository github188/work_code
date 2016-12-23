/*************************************************************************
    > File Name: main.c
    > Author: lizhu
    > Mail: 1489306911@qq.com 
    > Created Time: 2016年07月04日 星期一 16时04分56秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>
int main()
{
	unsigned char a[8];
	unsigned char b[8];
	int i;
	memset(a,0,8);
	memset(b,1,8);

	b[1]=1;
	b[2]=1;
	b[7]=1;
//	for(i=0;i<8;i++){
//		a[i]=b[i];
//	}
	memcpy(a,b,8);
	for(i=0;i<8;i++)
	{
		printf("-%d=%d-\n",i,a[i]);
	}
	return 0;
}
