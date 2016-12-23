/********************************************************************
* $ID: public.h             Mon 2016-09-19 15:29:29 +0800  lz       *
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
#ifndef  __PUBLIC_DEF_H
#define  __PUBLIC_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE                 (1024)
#define MAX_USER_CONNECTED              (4)

#define EVENT_COUNT                     (8)

#define DEF_PORT                        (9908)
#define DEF_IP                          "192.168.2.232"
#define SER_NUM                         "0123456789ABCDEF"

#define MAX_CMD_NUM         (50)
#define HEART_TIME          (3)


enum{
	STATE_NULL = 0,
	STATE_MCU_VER,
	STATE_UPGRADE,
};

typedef struct tagPROGPARAM{
	int           listport;
	char          net_ip[24];
	char          net_mac[24];
	char          sn[36];
}ParamInfo, *PrmIPtr;

typedef struct tagUSERINFO{
	int           sockfd;

	unsigned short recvLen, verified;
	unsigned char  recvBuf[MAX_BUFFER_SIZE];

	time_t         lastheart;
}UserInfo, *UserIPtr;

typedef struct tagPROGINFO{
	ParamInfo     param;
	UserInfo      users[MAX_USER_CONNECTED];
	
	int           fdListen;
	int           epollfd;
	struct        epoll_event events[EVENT_COUNT];

	unsigned char iMainState;
	unsigned char _rev[3];

	int           ttyfd;
	int           sendseq;
	int           sendBufLen;
	unsigned char sendBuf[MAX_BUFFER_SIZE];

	int           upg_offset;

	FILE          *fp;
	unsigned int  checksum;
	unsigned int  sumsize;

	unsigned int  msecNow;
	unsigned int  msecSend;

	int           iExit;
}ProgInfo, *ProgIPtr;

#define NPTO_MAGIC0                       (0xAA)
#define NPTO_MAGIC1                       (0x55)
#define NPTO_HEAD_SIZE                    (8)        // 协议头长度
#define NPTO_MAX_SIZE                     (512)      // 最大协议命令长度



typedef struct tagNETPROTOCOL {
	unsigned char  magic0;           // 恒定为0xAA
	unsigned char  magic1;           // 恒定为0x55
	unsigned char  checksum;         // 将本字段设置为0时,所有INT之和取BYTE。
	unsigned char  command;          // 命令字，命令字的最高位为1，表示反馈命令
	unsigned short cmdlen;           // 本条命令的长度
	unsigned short cmdseq;           // 命令序号，发送端标志，返回命令保持不变

	unsigned char  dat[NPTO_MAX_SIZE-NPTO_HEAD_SIZE];
}NetProto, *NetPtoPtr;

enum {
	CMD_ASK_VERSION  = 0x00,            //请求获取MCU的软件及硬件版本号
	CMD_ASK_UPGRADE,                    //核心板发送本命令，使得MCU进入升级模式。
	CMD_SEND_UPDATA,                    //核心板给MCU发送升级数据块
	CMD_SEND_VERSION,                   //核心板发送版本号给MCU
	CMD_GET_VERSION  = 0x80,            //得到MCU反馈的版本号
	CMD_GET_UPGRADE,                    //获取基板升级数据应答
	CMD_GET_UPDATARE,                   //获取MUC数据接收与写入的情况
	CMD_GET_VERSIONRE,                   //获取MCU是否成功接收版本号
};


enum {      // Parser buff status
	NPTO_PARSE_RESULT_SUCC = 0,

	NPTO_PARSE_RESULT_NOMAGIC=101,   // 没找到协议头
	NPTO_PARSE_RESULT_TOSMALL,       // 数据没有接收全
	NPTO_PARSE_RESULT_ERRCRC,        // 命令校验出错
	NPTO_PARSE_RESULT_UNKNOWN,

};

int prm_Default(ParamInfo *pinfo);
// 创建一个tcp服务和一个epollfd
int netio_create_tcpserver(ProgIPtr prog);


int prm_SetBoard(ParamInfo *pinfo);
void register_all_command(void);

int serial_new_connect(ProgIPtr prog, int fd);
int serial_open(const char *devname);

unsigned int misc_update_time(void);
unsigned int misc_current_time(void);
int netio_main_tcpserver(ProgIPtr prog);
int SendSerialCmd(ProgIPtr prog, int command, unsigned char *extdat, int extlen);

int serial_read(int fd, void *ptr, int len);

int process_command(ProgIPtr prog, int n, NetPtoPtr pcmd);

int npto_GenerateRaw(void *obuf, int *olen, int cmd, unsigned int cmdseq,
		    void *extdat, int extlen);

int npto_Parse(void *src, int ilen, NetPtoPtr cmdPkt, int *olen, int *result);


#endif// __PUBLIC_DEF_H
/******************** End Of File: public.h ********************/
