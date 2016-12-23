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
	int ret;
	unsigned char extdat[1];
	memset(extdat,0,1);

	extdat[0]=pcmd->dat[0];
	npto_GenerateRaw(prog->sendBuf, &prog->sendBufLen, 0x00 , pcmd->cmdseq, extdat, 1); 

	ret = write(prog->ttyfd, prog->sendBuf, prog->sendBufLen);
	PFUNC("Send %d ret=%d\n", prog->sendBufLen, ret);

		 
	return 0;	
}

static int proc_mcu_version_version(ProgIPtr prog, int n, NetPtoPtr pcmd)
{
	PFUNC("-----------OK!\n");
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
	reg_cmd_proc(CMD_ASK_VERSION, proc_mcu_version_version);
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
