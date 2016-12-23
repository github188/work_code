/********************************************************************
* $ID: main.c               Fri 2016-10-21 14:00:15 +0800  lz       *
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
#include <string.h>
#define MAX_CMD_NUM     (50)

#if 1//两种定义方式都可以
enum{
	FIRST    =0x01,
	TWO      =0x02,
};
#else
#define FIRST    (0x01)
#define TWO      (0x02)
#endif

typedef struct STUDENT{
	int num;
	char dat[20];
}Student, *Studentinfo;
//命令字处理函数
typedef int (*fcmd_proc)(Studentinfo std); //自己定义多少个参数和参数类型eg(Studentinfo std,int i, int h);

struct PROCESS_CMD{ //命令字处理结构体
	unsigned short  cmd_num;
	unsigned char   command[MAX_CMD_NUM];
	fcmd_proc process_cmd[MAX_CMD_NUM];
}proc_cmd;

//注册命令字处理函数
static void reg_cmd_proc(unsigned char command, fcmd_proc fproc)
{
	proc_cmd.command[proc_cmd.cmd_num] = command;
	proc_cmd.process_cmd[proc_cmd.cmd_num] = fproc;
	proc_cmd.cmd_num += 1;
}

static int proc_first_cmd(Studentinfo std)
{
	printf("1=  %d,%s\n",std->num,std->dat);
}

static int proc_two_cmd(Studentinfo std)
{
	printf("2=  %d,%s\n",std->num,std->dat);
}

//所有的命令字在这里注册
void register_all_command(void){
	reg_cmd_proc(FIRST, proc_first_cmd);
	reg_cmd_proc(TWO,   proc_two_cmd);
}

//查找并处理命令
int process_command(Studentinfo std)
{
	int i;
	for(i = 0; i< proc_cmd.cmd_num ;++i)
	{
		if(proc_cmd.command[i] == std->num)
		{
			printf("cmd : 0x%x\n",std->num);
			return proc_cmd.process_cmd[i](std);
		}
	}
}
int main(int argc,char *argv[])
{
	Student std;

	int flags = 1;
	register_all_command();//注册所有命令字
	
	while(1)
	{
		printf("请输入num和名字eg:33,xiaobing  :");
		scanf("%d,%s",&std.num,std.dat);
		process_command(&std);
	}
	return 0;
}



/********************* End Of File: main.c *********************/
