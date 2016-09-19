/********************************************************************
* $ID: NetIo.c              Mon 2016-09-19 20:46:23 +0800  lz       *
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

// 创建一个tcp服务和一个epollfd
int netio_create_tcpserver(ProgIPtr prog)
{
	int sockfd;
	struct sockaddr_in saddr;
	struct epoll_event ev;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		PERROR("Create socket error:");
		return -1;
	}

	int ope =1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		PERROR("setsockopt failed!");
		return -1;
	}
}


/********************* End Of File: NetIo.c *********************/
