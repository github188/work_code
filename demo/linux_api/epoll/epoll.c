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
#include "epoll.h"

ProgInfo gProgInfo;
int g_log_level = 0x000F;

int main(int argc, char *argv[])
{
	ProgIPtr prog = &gProgInfo;
	int sockfd;
	struct sockaddr_in saddr;
	struct epoll_event ev;
	int ret;
	char buf[1024];

	memset(prog, 0 ,sizeof(ProgInfo));

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
	
	prog->fdListen = sockfd; //listen socket fd;

	prog->epollfd = epoll_create(MAX_USER_CONNECTED);  //4表示创建epollfd连接的最大数量
	if(prog->epollfd == -1)
	{
		PERROR("Create epoll error:");
		return -1;
	}
	//添加监听描述符事件
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	if( epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
	{
		PERROR("create_tcpserver:epoll_ctl error: ");
		return -1;
	}

	while(1)
	{
		netio_main_tcpserver(prog);
	}

	return 0;
}

static int err_connect(ProgIPtr prog, int n)
{
	PERROR("epoll error: client closed\n");
	UserIPtr user = (UserIPtr) prog->events[n].data.ptr;
	close(user->sockfd);
	bzero(user, sizeof(UserInfo));

	return 0;
}

static int new_connect(ProgIPtr prog)
{
	int i, flags, connfd;
	struct epoll_event ev;
	struct sockaddr_in caddr;
	socklen_t client_size = sizeof(caddr);

	if((connfd = accept(prog->fdListen,(struct sockaddr *)&caddr, &client_size)) == -1)
	{
		PERROR("accept error:");
		return -1;
	}
	
	// 先检查已连接数量，
	for(i=0;i<MAX_USER_CONNECTED; ++i){
		if(prog->users[i].sockfd == 0)
		{
			break;// 还有可用的连接
		}
	}
	//// 连接已满
	if(i == MAX_USER_CONNECTED)
	{
		close(connfd);
		PERROR("Connection is full \n");
		return -1;
	}
	PERROR("ACCEPT......%d\n",i+1);

	flags = fcntl(connfd, F_GETFL, 0); // 设置为no block
	fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

	prog->users[i].sockfd = connfd;// 先加入后认证
	prog->users[i].lastheart = time(NULL);// 从现在开始计时

	// 加入到epollfd中去
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = &prog->users[i];
	if(epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
	{
		PERROR("epoll_ctl error:");
		return -1;
	}
	return 0;
}

//接收数据，保存到用户缓存中
static int recv_data(ProgIPtr prog, int n)
{
	UserIPtr user = (UserIPtr)prog->events[n].data.ptr;
	int buflen = user->recvLen, recvlen=0;
	int freelen = MAX_BUFFER_SIZE - buflen;
	
	if(freelen > 0)
	{
		if((recvlen = recv(user->sockfd, user->recvBuf+buflen, freelen, 0)) == -1)
			PERROR("Recv Failed: %s",strerror(errno));
		else if(recvlen == 0){//表示客户端关闭
			close(user->sockfd);
			bzero(user,sizeof(UserInfo));
			PERROR("Connection is closed!\n");
		}
		else
		{
			user->recvLen += recvlen;
			PERROR(">>>>>>>>>>>>>>>>>%s>>>>>>>>>>>>>>>>%d\n",user->recvBuf, user->recvLen);
			return 0;
		}
	}
	
	return -1;
}

// 从当前缓冲区中查找一条完整的命令
// // @src,ilen   查找缓冲区地址及大小
// // @cmdPkt     完整命令存取地址
// // @olen       缓冲区使用大小，注意它可能会大于当前命令的长度
// // @result     执行结果
//int npto_Parse(void *src, int ilen, NetPtoPtr cmdPkt, int *olen, int *result)
int npto_Parse(void *src, int ilen, int *olen, int *result)
{
	int i;
	unsigned char *ptr = (unsigned char*)src, checksum;

	for(i =0 ; i< ilen;i++){
		printf("%c ",ptr[i]);
	}
	printf("LEN=%d\n",ilen);

	*olen = ilen;
	*result = -1;
	return 0;
}

static int get_recv_buf(ProgIPtr prog, int n)
{
	int buflen = 0, result = 0;
	int recvlen = prog->users[n].recvLen;

	//从缓存中获取一条数据打印
	npto_Parse(prog->users[n].recvBuf, prog->users[n].recvLen, &buflen,&result);
	// 将获取过的或者错误的数据覆盖掉
	if (buflen > 0 && recvlen - buflen >= 0) {
		memmove(prog->users[n].recvBuf, prog->users[n].recvBuf + buflen, recvlen - buflen);
		prog->users[n].recvLen -= buflen;
	} 
	return result == 0 ? 0 : -1;
}

int netio_main_tcpserver(ProgIPtr prog)
{
	int i;
	int nfds; // 准备好的文件描述符数量
	int fd;
	
	// epoll wait 8ms
	nfds = epoll_wait(prog->epollfd, prog->events, EVENT_COUNT, 8);
	if(nfds == -1){
		PERROR("epoll_wait error!");
		return -1;
	}

	for(i = 0; i< nfds; ++i)
	{
		fd = prog->events[i].data.fd;
		if(prog->events[i].events & (EPOLLERR | EPOLLRDHUP))//// 有客户端关闭或者遇到错误
		{
			err_connect(prog,i);// 连接错误, 关闭连接
		}	
		else if(fd == prog->fdListen)// 有新的连接请求
		{
			new_connect(prog);
		}
		else if(prog->events[i].events & EPOLLIN)// 已经准备好，可用进行读写
		{
			recv_data(prog, i);
		}
	}
#if 1
	for(i = 0; i< MAX_USER_CONNECTED; ++i)// do command
	{
		if(!prog->users[i].sockfd || !prog->users[i].recvLen)
			continue;// 没有连接，或者没有数据的

		while(get_recv_buf(prog, i) != -1)
		{		
			if(!prog->users[i].verified){
				//get_recv_buf获取到的数据在这里进行处理
			}
		}
		
	}
#endif
}


/********************* End Of File: epoll.c *********************/
