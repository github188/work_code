/********************************************************************
* $ID: main.c               Mon 2016-09-19 15:27:41 +0800  lz       *
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "printdef.h"
#include "public.h"

ProgInfo gProgInfo;
int g_log_level = 0x000F;

int main(int argc, char *argv[])
{
	ProgIPtr prog = &gProgInfo;
	PVERSION("mbox prog");

	//TCP server close, but we send(...), it will Broken Pipe
	signal(SIGPIPE, SIG_IGN);

	memset(prog, 0, sizeof(ProgInfo));

	//加载配置文件
	prm_Default(&prog->param);

	//获取设备串口号
	prm_SetBoard(&prog->param);
	prog->iExit = 0;

	//create listen socket
	if(netio_create_tcpserver(prog) == -1)
	{
		PERROR("Create socket failed!\n");
		return -1;
	}
	
	//打开串口创建串口连接
	prog->ttyfd = serial_open(NULL);
	serial_new_connect(prog, prog->ttyfd);

	prog->iMainState = STATE_MCU_VER;

	while(1)
	{
		misc_update_time();
		prog->msecNow = misc_current_time();

		if(netio_main_tcpserver(prog) == 0){}
	
		switch(prog->iMainState) {
			case STATE_MCU_VER:
				if((prog->msecNow - prog->msecSend) > 2000) {
					SendSerialCmd(prog, CMD_ASK_VERSION, NULL, 0);
					prog->msecSend = prog->msecNow;
				}
				break;
		}
	}


}



/********************* End Of File: main.c *********************/
