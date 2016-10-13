/********************************************************************
* $ID: public.h      Wed 2012-12-12 19:59:19 +0800  dannywang       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <dannywang@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2012 ZHYTEK                                      *
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

#define  PATH_PARAM_FILE            "/dev/mtd5"

#define MAX_USER_CONNECTED            (4)
#define MAX_BUFFER_SIZE               (1024)

#define EVENT_COUNT                   (8)

#define DEF_PORT                      (9908)
#define DEF_IP                   "192.168.1.232"
#define SER_NUM                  "0123456789ABCDEF"

#define MAX_DDSCH_SUPPORT         (4)
#define MAX_RAM_DOTS              (1024)

#define VERSION_FILE                "/tmp/version.txt"
#define HEX_FILE                    "/tmp/mBox.hex"

typedef struct tagPROGPARAM {
    int            listport;              // tcp sock listen port
    char           net_ip[24];            // ip address
    char           net_mac[24];           // ip and mac for network

    char           sn[36];                // serial number
}ParamInfo, *PrmIPtr;


typedef struct tagUSERINFO {              // user connect me
    int           sockfd;

    unsigned short recvLen, verified;
    unsigned char  recvBuf[MAX_BUFFER_SIZE];

    time_t        lastheart;               // 用来标记最后一次收到心跳反馈的时间

}UserInfo, *UserIPtr;

enum {STATE_NULL=0, STATE_MCU_VER, STATE_UPGRADE, };
typedef struct tagPROGINFO {
    ParamInfo      param;

    unsigned char  iMainState;            // STATE
    unsigned char  _rev[3];

    int            fdListen;              // listen sock fd
    UserInfo       users[MAX_USER_CONNECTED];
    int            epollfd;        // epoll info
    struct epoll_event events[EVENT_COUNT];

    int            ttyfd;                 // rs232 fd
    int            sendseq;
    int            sendBufLen;
    unsigned char  sendBuf[MAX_BUFFER_SIZE];

	int            upg_offset;
	


    FILE           *fp;
    unsigned int   checksum;
    unsigned int   sumsize; 

    unsigned int   msecNow;
    unsigned int   msecSend;

    int            iExit;
}ProgInfo, *ProgIPtr;


//NetParser.h

#define NPTO_MAGIC0                       (0xAA)
#define NPTO_MAGIC1                       (0x55)
#define NPTO_HEAD_SIZE                    (8)        // 协议头长度
#define NPTO_MAX_SIZE                     (512)      // 最大协议命令长度

typedef struct tagNETPROTOCOL {
	unsigned char  magic0;           // 恒定为0xAA
	unsigned char  magic1;           // 恒定为0x55
	unsigned short cmdlen;           // 本条命令的长度
	unsigned char  command;          // 命令字，命令字的最高位为1，表示反馈命令
	unsigned char  checksum;         // 将本字段设置为0时,所有INT之和取BYTE。
	unsigned short cmdseq;           // 命令序号，发送端标志，返回命令保持不变

	unsigned char  dat[NPTO_MAX_SIZE-NPTO_HEAD_SIZE];
}NetProto, *NetPtoPtr;


enum {      // Parser buff status
	NPTO_PARSE_RESULT_SUCC = 0,
	
	NPTO_PARSE_RESULT_NOMAGIC=101,   // 没找到协议头
	NPTO_PARSE_RESULT_TOSMALL,       // 数据没有接收全
	NPTO_PARSE_RESULT_ERRCRC,        // 命令校验出错
	NPTO_PARSE_RESULT_UNKNOWN,

};


// 从当前缓冲区中查找一条完整的命令
// @src,ilen   查找缓冲区地址及大小
// @cmdPkt     完整命令存取地址
// @olen       缓冲区使用大小，注意它可能会大于当前命令的长度
// @result     执行结果
int npto_Parse(void *src, int ilen, NetPtoPtr cmdPkt, int *olen, int *result);

// 生成一条命令序列
// @obuf,olen  输出缓冲区地址及大小，同时也返回最后组成命令的长度
// @cmdcode    命令名称
// @cmdseq     当前命令的序号
// @extdat,extlen   当前命令的载体数据及长度
int npto_GenerateRaw(void *obuf, int *olen, int cmd, unsigned int cmdseq,
	void *extdat, int extlen);

// 生成一条反馈命令
int npto_GenerateResp(void *obuf, int *olen, NetPtoPtr cmdPkt, int result,
        void *extdat, int extlen);


//NetIo.c

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

// 创建一个tcp服务
int netio_create_tcpserver(ProgIPtr prog);

// 运行tcp服务
int netio_main_tcpserver(ProgIPtr prog);

// 心跳联接函数
int heart_beat(ProgIPtr prog);

// 关闭tcp服务
int netio_close(ProgIPtr prog);

enum {
    CMD_ASK_VERSION  = 0x01,            //请求获取MCU的软件及硬件版本号
    CMD_ASK_UPGRADE,                    //核心板发送本命令，使得MCU进入升级模式。
    CMD_SEND_UPDATA,                    //核心板给MCU发送升级数据块
    CMD_SEND_VERSION,                   //核心板发送版本号给MCU
    CMD_GET_VERSION  = 0x81,            //得到MCU反馈的版本号
    CMD_GET_UPGRADE,                    //获取基板升级数据应答
    CMD_GET_UPDATARE,                   //获取MUC数据接收与写入的情况
    CMD_GET_VERSIONRE                   //获取MCU是否成功接收版本号
};

//process_cmd.c

#define MAX_CMD_NUM         (50)
#define HEART_TIME          (3)

#define DO_CMD_SUCC         (0x00)

#define VERIFY_CMD          (0x01)
#define VERIFY_ERR          (0x81)

#define HEART_CMD           (0x02)
#define HEART_RESP          (0x82)

#define DEVICE_INFO         (0x03)
#define DEVICE_ERR          (0x83)

#define UPGRADE_DATA        (0x04)
#define UP_DATA_ERR         (0x84)

#define UPGRADE_CTRL        (0x05)
#define UP_CTRL_ERR         (0x85)

#define STD_WF_OUTPUT       (0x10)
#define STD_WF_ERR          (0x10|0x80)

#define UNSTD_WF_OUTPUT     (0x11)
#define UNSTD_WF_ERR        (0x11|0x80)

#define WF_START_OUTPUT     (0x12)
#define WF_START_ERR        (0x12|0x80)


// 注册所有的命令字和其处理函数
void register_all_command(void);

// 所有的命令处理函数
int process_command(ProgIPtr prog, int n, NetPtoPtr pcmd);

// copy to flash
int flashcp_main(const char *devicename, int devoffset, const char *filename, int quiet, int *r_percent);

// start all waveform ch on start
int waveform_restort(ProgIPtr prog);

// start wave interval
int waveform_looping(ProgIPtr prog);

//-------------------------------------------------------------
//            Save/Load param
//-------------------------------------------------------------
#define FIRM_USER_PATH          "/dev/mtd3"
#define FIRM_BAK_PATH           "/dev/mtd4"
#define PARAM_FILE_PATH         "/dev/mtd5"
int prm_Default(ParamInfo *pinfo);
int prm_Save(ParamInfo *pinfo, const char *fname);
int prm_Load(ParamInfo *pinfo, const char *fname);
int prm_SetBoard(ParamInfo *pinfo);

//-------------------------------------------------------------
//            Save Status
//-------------------------------------------------------------
int savstat_start(ProgIPtr prog);


unsigned int misc_update_time(void);
unsigned int misc_current_time(void);
unsigned int misc_read_serialGPIO(void);


extern ProgInfo gProgInfo;
int serial_open(const char *devname);
int serial_new_connect(ProgIPtr prog, int fd);
int serial_read(int fd, void *ptr, int len);
void serial_close(int fd);
int SendSerialCmd(ProgIPtr prog, int command, unsigned char *extdat, int extlen);
int SendSerialUpgrade(ProgIPtr prog, int command, unsigned char *extdat, int extlen);

int check_update_file(ProgIPtr prog);

#endif// __PUBLIC_DEF_H
/******************** End Of File: public.h ********************/
