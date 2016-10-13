/********************************************************************
* $ID: epoll.c              Thu 2016-09-29 09:25:33 +0800  lz       *
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


int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in saddr;
	struct epoll_event ev;
	int epollfd;
	struct epoll_event events[8];   //8 表示epoll时间的最大数量
	int ret;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		PERROR("create socket error:");
		return -1;
	}

	int opt = 1; //解决地址复用关系
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
	{
		PERROR("setsockopt failed!");
		return -1;
	}

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(1234);

	if(bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		PERROR("bind error:");
		return -1;
	}

	if(listen(sockfd , 5) == -1)
	{
		PERROR("bind error:");
		return -1;
	}

	epollfd = epoll_create(4);  //4表示创建epollfd连接的最大数量
	if(epollfd == -1)
	{
		PERROR("Create epoll error:");
		return -1;
	}

	//添加事件
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	if( epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
	{
		PERROR("create_tcpserver:epoll_ctl error: ");
		return -1;
	}

	while(1)
	{
		ret = epoll_wait(epollfd, events);
	}

	return 0;
}



/********************* End Of File: epoll.c *********************/
