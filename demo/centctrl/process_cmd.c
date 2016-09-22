/********************************************************************
* $ID: process_cmd.c        Thu 2016-09-22 15:12:55 +0800  lz       *
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
#include "public.h"
#include "printdef.h"

typedef int (*fcmd_proc)(ProgIPtr prog, int n, NetPtoPtr pcmd);

struct PROCESS_CMD{
	unsigned short cmd_num;
	unsigned char command[MAX_CMD_NUM];
	fcmd_proc process_cmd[MAX_CMD_NUM];
}proc_cmd;

static void reg_cmd_proc(unsigned char command, fcmd_proc fproc)
{
	proc_cmd.command[proc_cmd.cmd_num] = command;
	proc_cmd.process_cmd[proc_cmd.cmd_num] = fproc;
	proc_cmd.cmd_num += 1;
}

static int proc_mcu_version(ProgIPtr prog, int n, NetPtoPtr pcmd)
{
    FILE * file;
    int len;
    int i;
    unsigned char DataBuffer[25];
    unsigned char version[12];
    unsigned char recv_version[12];
    printf("-----------------\n");
    file = fopen("/lib/ko/version.txt", "r");
    if(file == NULL)
    {   
        printf("file open error!\n");
        return -1; 
    }   
    len = fread(DataBuffer,1,sizeof(DataBuffer),file); //读取文件到DataBuffer中
    printf("file size ->%d \n",len);
    fclose(file);

    memcpy(version, DataBuffer,12);
    memcpy(recv_version, pcmd->dat,12);
    
    printf("version= %s recv_version= %s\n", version, recv_version);
    for(i=0;i<12;i++)
    {   
        printf("[%x]",version[i]);
    }   
    printf("\n");
    for(i=0;i<12;i++)
    {   
        printf("[%x]",recv_version[i]);
    }   
    printf("\n");
    if(memcmp(version,recv_version,12) !=0)
    {   
        printf("update wait ok \n");
        prog->iMainState = STATE_MCU_VER;
        prog->upg_offset = 0;
    }   
    else
    {   
		printf("version like \n");
        prog->iMainState = STATE_MCU_VER;
    }   
    return 0;	
}

int process_command(ProgIPtr prog, int n, NetPtoPtr pcmd)
{
	int i;
	for(i = 0; i< proc_cmd.cmd_num; ++i){
		if(proc_cmd.command[i] == pcmd->command)
		{
			PFUNC("cmd: 0x%x\n", pcmd->command);
			return proc_cmd.process_cmd[i](prog, n, pcmd);
		}
	}
}

void register_all_command(void)
{
	reg_cmd_proc(CMD_GET_VERSION, proc_mcu_version);
}

int SendSerialCmd(ProgIPtr prog, int command, unsigned char *extdat, int extlen)
{
	int ret;

	npto_GenerateRaw(prog->sendBuf, &prog->sendBufLen, command, prog->sendseq++, extdat, extlen);
	ret = write(prog->ttyfd, prog->sendBuf, prog->sendBufLen);
	PFUNC("Send %d ret=%d\n", prog->sendBufLen, ret);

	return ret;
}

/****************** End Of File: process_cmd.c ******************/
