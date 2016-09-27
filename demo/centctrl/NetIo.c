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


static int recv_data(ProgIPtr prog, int n)
{
	UserIPtr user = (UserIPtr)prog->events[n].data.ptr;
	int buflen = user->recvLen, recvlen = 0;
	int freelen = MAX_BUFFER_SIZE - buflen;

	if(freelen > 0)
	{
		if((recvlen = recv(user->sockfd,\
					user->recvBuf+buflen, freelen, 0)) == -1)
		{
			PERROR("Recv Failed : %s",strerror(errno));
		}
		else if(recvlen == 0)
		{
			close(user->sockfd);
			bzero(user, sizeof(UserInfo));
			PERROR("Connection is closed!\n");
		}
		else
		{
			user->recvLen +=recvlen;
			return 0;
		}
	}

	return -1;
	
}

static int err_connect(ProgIPtr prog, int n)
{
	PERROR("epoll error: client closed\n");

	UserIPtr user = (UserIPtr) prog->events[n].data.ptr;
	close(user->sockfd);
	bzero(user, sizeof(UserInfo));

	return 0;
}

// 创建一个tcp服务和一个epollfd
int netio_create_tcpserver(ProgIPtr prog)
{
	int sockfd;
	struct sockaddr_in saddr;
	struct epoll_event ev;
	int opt = 1;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		PERROR("Create socket error:");
		return -1;
	}
	
	//解除端口复用
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		PERROR("setsockopt failed!");
		return -1;
	}


	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(prog->param.listport);

	if(bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		PERROR("bind error: ");
		return -1;
	}

	if(listen(sockfd, 5) == -1)
	{
		PERROR("listen error :");
		return -1;
	}

	prog->fdListen = sockfd;

	//创建epollfd 
	prog->epollfd = epoll_create(MAX_USER_CONNECTED);
	if(prog->epollfd == -1)
	{
		PERROR("Create epoll error: ");
		return -1;
	}
	
	//把sockfd加到epollfd中去
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	if(epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
	{
		PERROR("create_tcpserver:epoll_ctl error :");
		return-1;
	}

	//注册所有的命令字
	register_all_command();

	return 0;
}

static int new_connect(ProgIPtr prog)
{
	int j, flags, connfd;
	struct epoll_event ev;
	struct sockaddr_in caddr;
	socklen_t client_size = sizeof(caddr);

	if((connfd = accept(prog->fdListen, \
					
					(struct sockaddr *)&caddr, &client_size)) == -1)
	{
		PERROR("accept error: ");
		return -1;
	}

	for(j = 0; j< MAX_USER_CONNECTED ; j++){
		if(prog->users[j].sockfd == 0)
			break;
	}

	if(j == MAX_USER_CONNECTED)
	{
		close(connfd);
		PERROR("Connection is full!\n");
		return -1;
	}
	PERROR("Accept.. %d\n", j+1);

	flags = fcntl(connfd, F_GETFL, 0);
	fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

	prog->users[j].sockfd = connfd;
	prog->users[j].lastheart = time(NULL);

	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = &prog->users[j];
	if(epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
	{
		PERROR("epoll_ctl error:");
		return -1;
	}

	return 0;
}

static int get_command(ProgIPtr prog, int n, NetPtoPtr pcmd)
{
	int buflen = 0, result = 0;
	int recvlen = prog->users[n].recvLen;

	if(recvlen < NPTO_HEAD_SIZE)
		return -1; //获取到的长度不够

	//从缓存中获取一条命令
	npto_Parse(prog->users[n].recvBuf, prog->users[n].recvLen, pcmd, &buflen, &result);

	//将获取过的或者错误的数据覆盖掉
	if(buflen > 0 && recvlen -buflen >=0)
	{
		memmove(prog->users[n].recvBuf, prog->users[n].recvBuf+buflen, recvlen-buflen);
		prog->users[n].recvLen -= buflen;
	}

	return result == NPTO_PARSE_RESULT_SUCC ? 0: -1;
}

//创建一个tcp服务和一个epollfd 
int netio_main_tcpserver(ProgIPtr prog)
{
	int i, nfds;
	NetProto cmd;
	
	//该函数返回已经准备好的描述符时间数目
	nfds = epoll_wait(prog->epollfd, prog->events, EVENT_COUNT, 8);
	if(nfds == -1)
	{
		PERROR("epoll_wait error !");
		return -1;
	}

	//进行遍历，这里只要遍历已经准备好的IO时间
	for(i = 0; i< nfds; ++i)
	{
		//根据描述符的类型和事件类型进行处理
		if(prog->events[i].events &(EPOLLERR | EPOLLRDHUP)) //有客户端关闭或者遇到错误
		{
			err_connect(prog,i);//连接错误，关闭连接
		}
		else if(prog->events[i].data.fd == prog->fdListen) // 有新的连接请求
		{
			new_connect(prog);
		}
		else if(prog->events[i].events & EPOLLIN) //已准备好，可以进行读写操作
		{
			UserIPtr user = (UserIPtr)prog->events[i].data.ptr;
			if(user->sockfd == prog->ttyfd)
			{
				int len = serial_read(prog->ttyfd, &user->recvBuf[user->recvLen],MAX_BUFFER_SIZE-user->recvLen);
				if(len >0) user->recvLen +=len;
			}
			else
				recv_data(prog,i);
		}
	}

	for(i = 0; i< MAX_USER_CONNECTED; ++i) // do command 
	{
		if(!prog->users[i].sockfd || !prog->users[i].recvLen)
		{
			continue;  //没有连接，或者没有数据的
		}

		while(get_command(prog, i, &cmd) != -1)
		{
			if(!prog->users[i].verified)
			{
				if(process_command(prog,i,&cmd) == -1)
				{
					PERROR("verified failed! \n");
					break;
				}
				else
				{
					PERROR("verified success!\n");
					continue;
				}
			}
			if(process_command(prog, i ,&cmd) == -1)
			{
				PERROR("process command error !\n");
			}
		}
	}

	if(nfds < 1) return -2;

	return 0;
}


/********************* End Of File: NetIo.c *********************/
