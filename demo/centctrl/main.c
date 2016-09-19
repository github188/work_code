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

	signal(SIGPIPE, SIG_IGN);

	memset(prog, 0, sizeof(ProgInfo));

	prm_Default(&prog->param);

	prm_SetBoard(&prog->param);
	prog->iExit = 0;

	if(netio_create_tcpserver(prog) == -1)
	{
		PERROR("Create socket failed!\n");
		return -1;
	}

	prog->ttyfd = serial_open(NULL);
	serial_new_connect(prog, prog->ttyfd);


}



/********************* End Of File: main.c *********************/
