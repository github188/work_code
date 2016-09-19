/********************************************************************
* $ID: SerialIo.c    Fri 2014-06-06 16:13:18 +0800  dannywang       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <dannywang@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2014 ZHYTEK                                         *
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
#include "rs232.h"
#include "printdef.h"

char g_rs232_dev[16];

int serial_open(const char *devname)
{
    int fd;

    if(!devname) devname = g_rs232_dev;

    if((fd = OpenCom(devname, 57600, 'n', 8, 1, 0, 0, 0)) < 0) {
	printf("Error open dev:%s\n", devname);
	return -1;
    }

    return fd;
}


// add searial fd to POLL
int serial_new_connect(ProgIPtr prog, int fd)
{
    int j, flags;
    struct epoll_event ev;

    // 先检查已连接数量，
    for (j = 0; j < MAX_USER_CONNECTED; ++j) {
	if (prog->users[j].sockfd == 0) {
	    break; // 还有可用的连接
	}
    }
    // 连接已满
    if (j == MAX_USER_CONNECTED) {
	PERROR("Connection is full!\n");
	return -1;
    }

    flags = fcntl(fd, F_GETFL, 0);              // 设置为no block
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    prog->users[j].sockfd = fd;             // 先加入后认证
    prog->users[j].lastheart = time(NULL);      // 从现在开始计时
    prog->users[j].verified = 1;

    // 加入到epollfd中去
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = &prog->users[j];
    if (epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
	PERROR("epoll_ctl error: ");
	return -1;
    }

    return 0;
}


int serial_read(int fd, void *ptr, int len)
{
    int rdlen = read(fd, ptr, len);
    return rdlen;
}

void serial_close(int fd)
{
    if(fd > 0)
	CloseCom(fd);
}

/******************* End Of File: SerialIo.c *******************/
