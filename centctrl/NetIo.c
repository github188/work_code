/********************************************************************
* $ID: NetIo.c       Mon 2013-08-05 19:24:00 +0800  dannywang       *
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

#include "public.h"
#include "printdef.h"

#if 0
static void print_cmd(NetProto cmd) {
    PERROR ("cmd.magic0:\t0x%x\n", cmd.magic0);
    PERROR ("cmd.magic1:\t0x%x\n", cmd.magic1);
    PERROR ("cmd.cmdlen:\t%d\n", cmd.cmdlen);
    PERROR ("cmd.command:\t0x%x\n", cmd.command);
    PERROR ("cmd.cmdseq:\t0x%x\n", cmd.cmdseq);
    if (cmd.command > 0x80) {
	PERROR ("cmd.dat[0]\t0x%x\n", cmd.dat[0]);
	//PERROR ("cmd.dat+1:\t%s\n", cmd.dat + 1);
    }else {
	cmd.dat[cmd.cmdlen-NPTO_HEAD_SIZE] = 0;
	PERROR ("cmd.dat:\t%s\n", cmd.dat);
    }
}
#endif

// 接收数据，保存到用户缓存中
static int recv_data(ProgIPtr prog, int n)
{

    UserIPtr user = (UserIPtr)prog->events[n].data.ptr;
    int buflen = user->recvLen, recvlen= 0;
    int freelen = MAX_BUFFER_SIZE -buflen;


    if (freelen > 0) { 
	if ((recvlen = recv(user->sockfd, \
			user->recvBuf+buflen, freelen, 0)) == -1) {
	    PERROR("Recv Failed: %s", strerror(errno));
	}else if (recvlen == 0) { // 客户端关闭
	    close(user->sockfd);
	    bzero(user, sizeof(UserInfo));
	    PERROR("Connection is closed!\n");
	}else {
	    user->recvLen += recvlen;
	    //PERROR(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n", user->recvLen);
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

static int new_connect(ProgIPtr prog)
{
    int j, flags, connfd;
    struct epoll_event ev;
    struct sockaddr_in caddr;
    socklen_t client_size = sizeof(caddr);

    if ((connfd = accept(prog->fdListen, \
		    (struct sockaddr *)&caddr, &client_size)) == -1) {
	PERROR("accept error: ");
	return -1;
    }

    // 先检查已连接数量，
    for (j = 0; j < MAX_USER_CONNECTED; ++j) {
	if (prog->users[j].sockfd == 0) {
	    break; // 还有可用的连接
	}
    }
    // 连接已满
    if (j == MAX_USER_CONNECTED) {
	close(connfd);
	PERROR("Connection is full!\n");
	return -1;
    }
    PERROR("Accept....%d\n", j+1);

    flags = fcntl(connfd, F_GETFL, 0); // 设置为no block
    fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

    prog->users[j].sockfd = connfd;             // 先加入后认证
    prog->users[j].lastheart = time(NULL);      // 从现在开始计时

    // 加入到epollfd中去
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = &prog->users[j];
    if (epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
	PERROR("epoll_ctl error: ");
	return -1;
    }

    return 0;
}

static int get_command(ProgIPtr prog, int n, NetPtoPtr pcmd)
{

    int buflen = 0, result = 0;
    int recvlen = prog->users[n].recvLen;

    if (recvlen < NPTO_HEAD_SIZE)
	return -1; // 长度不够

    // 从缓存中获取一条命令
    npto_Parse(prog->users[n].recvBuf, prog->users[n].recvLen, pcmd, &buflen, &result); 

    // 将获取过的或者错误的数据覆盖掉
    if (buflen > 0 && recvlen - buflen >= 0) {
	memmove(prog->users[n].recvBuf, prog->users[n].recvBuf + buflen, recvlen - buflen);
	prog->users[n].recvLen -= buflen;
    }

    return result == NPTO_PARSE_RESULT_SUCC ? 0 : -1;
}


/////////////////////////////////////////////////////////////////////
// 创建一个tcp服务和一个epollfd
int netio_create_tcpserver(ProgIPtr prog)
{
    int sockfd;
    struct sockaddr_in saddr;
    struct epoll_event ev;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	PERROR("Create socket error:");
	return -1;
    }

    int opt = 1; // 地址复用
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
	PERROR("setsockopt failed!");
	return -1;
    }

    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(prog->param.listport);

    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
	PERROR("bind error: ");
	return -1;
    }

    if (listen(sockfd, 5) == -1) {
	PERROR("listern error: ");
	return -1;
    }

    prog->fdListen = sockfd; // listen socket fd

    // 创建epollfd
    prog->epollfd = epoll_create(MAX_USER_CONNECTED);
    if (prog->epollfd == -1) {
	PERROR("Create epoll error: ");
	return -1;
    }

    // 把sockfd加到epollfd中去
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (epoll_ctl(prog->epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
	PERROR("create_tcpserver:epoll_ctl error: ");
	return -1;
    }

    register_all_command(); // 注册所有命令字

    return 0;
}

int netio_main_tcpserver(ProgIPtr prog)
{
    int i, nfds; // 准备好的文件描述符数量
    NetProto cmd;

    // epoll wait 8ms
    nfds = epoll_wait(prog->epollfd, prog->events, EVENT_COUNT, 8);
    if (nfds == -1) {
	PERROR("epoll_wait error!");
	return -1;
    }

    for (i = 0; i < nfds; ++i) {
	if (prog->events[i].events & (EPOLLERR | EPOLLRDHUP)) { // 有客户端关闭或者遇到错误
	    err_connect(prog, i); // 连接错误, 关闭连接

	}
	else if (prog->events[i].data.fd == prog->fdListen) { // 有新的连接请求
	    new_connect(prog);

	}
	else if (prog->events[i].events & EPOLLIN) { // 已经准备好，可用进行读写
	    UserIPtr user = (UserIPtr)prog->events[i].data.ptr;
	    if(user->sockfd == prog->ttyfd) {
		int len = serial_read(prog->ttyfd, &user->recvBuf[user->recvLen], MAX_BUFFER_SIZE-user->recvLen);
		if(len > 0) user->recvLen += len;
	    }
	    else
		recv_data(prog, i);
	}
    }

    for (i = 0; i < MAX_USER_CONNECTED; ++i) { // do command
	if (!prog->users[i].sockfd || !prog->users[i].recvLen) {
	    continue;     // 没有连接，或者没有数据的
	}

	while (get_command(prog, i, &cmd) != -1) {
	    if (!prog->users[i].verified) {
		cmd.command = VERIFY_CMD;
		if (process_command(prog, i, &cmd) == -1) {
		    PERROR("verified failed !\n");
		    break;
		}else {
		    PERROR("verified success!\n");
		    continue;
		}
	    }
	    if (process_command(prog, i, &cmd) == -1) {
		PERROR("process command error!\n");
		// print_cmd(cmd);
	    }
	}
    }

    if(nfds < 1) return -2;

    return 0;
}

// 心跳联接函数, 先检测是否已经连接超时
int heart_beat(ProgIPtr prog) { // 每十秒进行一次心跳连接
    int i;
    NetProto  cmd;
    unsigned char keepalive[12] = "KEEPALIVE\0\0\0";
    static unsigned short cmdseq[MAX_USER_CONNECTED];

    for (i = 0; i < MAX_USER_CONNECTED; ++i) {
	int buflen = NPTO_MAX_SIZE;

	if(prog->users[i].sockfd == prog->ttyfd) continue;

	if (prog->users[i].sockfd != 0) { // 已连接且已认证的才发送心跳连接
	    if (time(NULL) - prog->users[i].lastheart > 3*HEART_TIME) {
		// 已经超时了
		cmdseq[i] = 0;
		close(prog->users[i].sockfd);
		bzero(prog->users + i, sizeof(prog->users[i]));
		PERROR("TIMEOUT, Connection is closed!\n");

	    }
	    else if (prog->users[i].verified) { // 还没有超时
		//PERROR("Sending heart beat!\n");
		npto_GenerateRaw(&cmd, &buflen, HEART_CMD, cmdseq[i]++, keepalive, 12);
		if (send(prog->users[i].sockfd, &cmd, buflen, 0) == -1) {
		    PERROR("send_cmd error!\n");
		    continue;
		}
	    }
	}
    }
    return 0;
}

// 关闭所有的文件描述符，释放锁
int netio_close(ProgIPtr prog)
{
    int i;
    for (i = 0; i < MAX_USER_CONNECTED; ++i) {
	if (prog->users[i].sockfd != 0)
	    close(prog->users[i].sockfd);
    }

    close(prog->fdListen);
    close(prog->epollfd);

    return 0;
}






/********************* End Of File: NetIo.c *********************/
