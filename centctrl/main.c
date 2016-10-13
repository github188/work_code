/********************************************************************
* $ID: main.c        Mon 2013-08-05 19:34:36 +0800  dannywang       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <dannywang@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2013 ZHYTEK                                         *
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

#include "public.h"
#include "printdef.h"


ProgInfo gProgInfo;

int g_log_level = 0x000F;

int parse_args(int argc, char *argv[], void *dat);

int main(int argc, char *argv[])
{
    ProgIPtr prog = &gProgInfo;
    PVERSION("mbox prog");

    // TCP server close, but we send(...), it will Broken Pipe
    signal(SIGPIPE, SIG_IGN);

    memset(prog, 0, sizeof(ProgInfo));

    // load param from file
    prm_Default(&prog->param);
//    if (prm_Load(&prog->param, PARAM_FILE_PATH) < 0) {
//        PERROR("load param failed, Set Default\n");
//    }
 
    parse_args(argc, argv, prog);
    
    prm_SetBoard(&prog->param);

    prog->iExit = 0;

    // create listen socket
    if(netio_create_tcpserver(prog) == -1) {
	PERROR("Create socket Failed!\n");
	return -1;
    }

    prog->ttyfd = serial_open(NULL);
    serial_new_connect(prog, prog->ttyfd);

    prog->iMainState = STATE_MCU_VER;

	check_update_file(prog);       //检查是否有升级文件存在
    while(1) {
	misc_update_time();
	prog->msecNow = misc_current_time();
	if(netio_main_tcpserver(prog) == 0) {        // recv some message

	}
	
	switch(prog->iMainState) {
	case STATE_MCU_VER:
	    if((prog->msecNow - prog->msecSend) > 3000) {
		SendSerialCmd(prog, CMD_ASK_VERSION, NULL, 0);
		prog->msecSend = prog->msecNow;
	    }
	    break;
	case STATE_UPGRADE:
	    if((prog->msecNow - prog->msecSend) > 2000) {
			SendSerialUpgrade(prog, CMD_SEND_UPDATA, NULL, 0);
			prog->msecSend = prog->msecNow;
		}
	    break;
	}
    }

    // close socket
    netio_close(prog);
    serial_close(prog->ttyfd), prog->ttyfd = 0;

    return 0;
}

void Usage(const char *prog, const char *error)
{
    fprintf(stderr, "%s\n\t"
	    "Usage %s \n\t"
	    "--log-level=0x         -- set debug print level\n\t"
	    "--upgrade devname filename\n\t"
	    "--ipaddr ipaddr mac    -- set network info\n\t"
	    "\n", error, prog);

}

#include <getopt.h>
int g_check_image = 1;
int parse_args(int argc, char *argv[], void *dat)
{
    char *ptr;
    ParamInfo *pinfo = &((ProgIPtr)dat)->param;
    int c, iIndex=0;
    enum {OPT_LOGLEV=127, OPT_UPGRADE, OPT_IP_MAC};
    struct option opts[] = {
	{"log-level", 1, 0, OPT_LOGLEV}, 
	{"upgrade", 2, 0, OPT_UPGRADE},
	{"ipaddr", 2, 0, OPT_IP_MAC},
	{NULL,        0, NULL, 0},
    };

    // set loglevel from env
    if((ptr = getenv("MVSGDEBUG_LOGLEVEL")))
	g_log_level = strtoul(ptr, (char**)NULL, 16);

    // pause args
    while((c = getopt_long(argc, argv, "", opts, &iIndex)) >= 0) {
	switch(c) {
	    case OPT_LOGLEV:
		if(optarg[0] == '0' && optarg[1] == 'x')
		    g_log_level = strtoul(optarg, (char**)0, 16);
		else
		    g_log_level = atoi(optarg);
		break;
	    case OPT_UPGRADE:
		if(argc > optind+1) {
		    if(index(argv[optind], '3'))
			flashcp_main(FIRM_USER_PATH, 0, argv[optind+1], 1, NULL);
		    if(index(argv[optind], '4'))
			flashcp_main(FIRM_BAK_PATH, 0, argv[optind+1], 1, NULL);
		    exit(0);
		}
		else {
		    Usage(argv[0], "Error Upgrade Param");
		    exit(0);
		}
		break;
	    case OPT_IP_MAC:
		if(argc > optind) {
		    snprintf(pinfo->net_ip, sizeof(pinfo->net_ip), "%s", argv[optind]);

		    if(argc > optind+1)
			snprintf(pinfo->net_mac, sizeof(pinfo->net_mac), "%s", argv[optind+1]);

		    prm_Save(pinfo, PARAM_FILE_PATH);
		    PFUNC("Set New Ip:%s[%s]\n", pinfo->net_ip, pinfo->net_mac);
		}
		exit(0);
		break;
	    default:
		Usage(argv[0], "Unknown Param");
		exit(-1);
	}
    }

    if(argc != optind) {
	PERROR("Argc=%d, optind=%d\n", argc, optind);
	Usage(argv[0], "Error Param");
	exit(-2);
    }

    return 0;
}

/********************* End Of File: main.c *********************/
