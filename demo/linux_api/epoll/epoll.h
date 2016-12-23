/********************************************************************
* $ID: epoll.h              Thu 2016-10-20 15:10:24 +0800  lz       *
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
#ifndef  __EPOLL_DEF_H
#define  __EPOLL_DEF_H

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
#include <sys/socket.h>
#include "printdef.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_USER_CONNECTED     (4)
#define MAX_BUFFER_SIZE        (1024)

#define EVENT_COUNT            (8)

typedef struct tagUSERINFO{
    int sockfd;
    unsigned short recvLen,verified;
    unsigned char recvBuf[MAX_BUFFER_SIZE];
	time_t        lastheart;   // 用来标记最后一次收到心跳反馈的时间
}UserInfo, *UserIPtr;

typedef struct tagPROGINFO{
    int            fdListen;
    UserInfo       users[MAX_USER_CONNECTED];
    int            epollfd;
    struct epoll_event  events[EVENT_COUNT];
}ProgInfo, *ProgIPtr;

extern ProgInfo gProgInfo;
int netio_main_tcpserver(ProgIPtr prog);

#endif// __EPOLL_DEF_H
/********************* End Of File: epoll.h *********************/
