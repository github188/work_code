/*************************************************************************
    > File Name: socket_select.c
    > Author: lizhu
    > Mail: 1489306911@qq.com 
    > Created Time: 2016年08月15日 星期一 07时27分57秒
 ************************************************************************/

#include<stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <asm-generic/socket.h> 
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define MYPORT  1234
#define BACKLOG 5
#define WEB_ROOT "./www"
#define DEFAULT_PAGE "/home.html"
#define MAX_IDLECONNCTIME 2;

int conn_amount = 0;
int fd_A[BACKLOG]; 

void showclient()
{
	int i;
	printf("client amount: %d\n", conn_amount);
	for (i = 0; i < BACKLOG; i++) {
		printf("[%d]:%d  ", i, fd_A[i]);
	}
	printf("\n\n");
}

void do_get(int sockfd,char *filename){
    char path[128] = {'\0'};
    if(strcmp("/",filename)==0){//这个操作时默认首页的时候使用的
        filename = DEFAULT_PAGE;//这个时可以的
    }
    sprintf(path,"%s%s",WEB_ROOT,filename);
    int ret = open(path,O_RDONLY);
    if(ret<0){
        perror("open");
        char err_msg[129] = "<h1>404</h1>";
        send(sockfd,err_msg,strlen(err_msg),0);
        return;
    }

    while(1){
        char buf[1024] = {'\0'};
        int size = read (ret,buf,sizeof(buf));
        if(size <=0 )break;
        write(sockfd,buf,size);
    }
    close(ret);
}

void main()
{
	signal(SIGPIPE, SIG_IGN);
	
	int opt = 1;
	int sock_fd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int ret = 1;
	char buf[BUF_SIZE];
	int i;
	fd_set fdsr;
	int maxsock;
	struct timeval tv;
	int con_time[BUF_SIZE];

	int Timeout = 2000;//2秒
	time_t timep;
	struct tm *p;
	int flag_minutechange = 0;
	int lastminute,newminute;

	//获取系统时间
	time(&timep);
	p = gmtime(&timep);
	
	//建立socket套接字 
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		return;
	}

	//解决服务器端口占用问题 ret=1表示可以重用
	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &ret, sizeof(int)) == -1){  
		perror("setsockopt");
		return;
	}

    //设置发送时限
    setsockopt(sock_fd,SOL_SOCKET,SO_SNDTIMEO,(char *)&Timeout,sizeof(int) );
    //设置接收时限
    setsockopt(sock_fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&Timeout,sizeof(int));

	//设置本机的服务类型
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(MYPORT);//端口设定
	server_addr.sin_addr.s_addr = INADDR_ANY;//设置IP地址
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

	//绑定本机IP地址和端口号
	if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("bind");
		return;
	}

	//监听客户端连接
	if(listen(sock_fd, BACKLOG) == -1){
		perror("listen");
		return;
	}

	printf("listen port %d\n", MYPORT);
	printf("sock_fd =  %d \n",sock_fd);
	
	conn_amount = 0;
	sin_size = sizeof(client_addr);
	maxsock = sock_fd;

	//时间赋值以此为基点判断超时连接
	lastminute = p->tm_min;

	while(1){
	
		//定时器比较
		time(&timep);
		p = gmtime(&timep);
		newminute = p->tm_min;
		printf("%d --- %d   \n",lastminute , newminute);
		if(newminute != lastminute){
			lastminute = newminute;
			flag_minutechange = 1;
		}

        /*将指定的文件描述符集清空，在对文件描述符集合进行设置前，
         * 必须对其进行初始化，如果不清空，由于在系统分配内存空间后，
         * 通常并不作清空处理，所以结果是不可知的。
         * */
		FD_ZERO(&fdsr);
        /*
         *用于在文件描述符集合中增加一个新的文件描述符。
         * */
		FD_SET(sock_fd, &fdsr);

		tv.tv_sec = 10;
		tv.tv_usec = 0;

		//将存在的套接字加入描述符集
		for(i = 0; i< BACKLOG; i++){
			if(fd_A[i] != 0){
				FD_SET(fd_A[i], &fdsr);
		printf("%d --- %d -------flag_minutechange=%d  \n",lastminute , newminute,flag_minutechange);
				if(flag_minutechange == 1){
					con_time[i]--;
					if(con_time[i] <=0 ){
						printf("-----------------i=%d===========\n",i);
						close(fd_A[i]);
						FD_CLR(fd_A[i], &fdsr);
						fd_A[i] = 0;
						conn_amount--;
					}
				}
			}
		}
		
		flag_minutechange = 0;

		//select函数用来探测套接字是否有数据异常
		ret = select(maxsock+1, &fdsr, NULL, NULL, &tv);
		if(ret < 0){
			perror("select");
			break;
		}else if( ret == 0){
			printf("timeout\n");
			continue;
		}

        /*
         * FD_ISSET(int fd,fd_set *fdset) 用于测试指定的文件描述符是否在该集合中。
         * */
		for(i = 0; i< conn_amount; i++){
			if(FD_ISSET(fd_A[i], &fdsr)){
				ret = recv(fd_A[i], buf, sizeof(buf), 0);
				if(ret <= 0){ //ret <= 0 表明客户端关闭连接，服务器也关闭相应连接，并把连接套接子从文件描述符集中清除
					printf("client[%d] close\n",i);
					close(fd_A[i]);
                    /*
                     *FD_CLR(fd_set *fdset) 用于在文件描述符集合中删除一个文件描述符。
                     * */
					FD_CLR(fd_A[i], &fdsr);
					fd_A[i] = 0;
					conn_amount--;
				}else{//否则表明客户端有数据发送过来，作相应接受处理
					con_time[i] = MAX_IDLECONNCTIME;
					if(ret < BUF_SIZE)
						memset(&buf[ret], '\0', 1);
                    
					printf("client[%d] send:%s\n",i,buf);
				}
			}
		}
	printf("sock_fd =  %d \n",sock_fd);

		//// 以下说明异常有来自客户端的连接请求
		if(FD_ISSET(sock_fd, &fdsr)){
			new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
			if(new_fd<=0){
				perror("accept");
				continue;
			}
			// 判断活跃连接数时候是否小于最大连接数，如果是，添加新连接到文件描述符集中
			if(conn_amount < BACKLOG){

				for(i = 0; i<BACKLOG; i++){
					if(fd_A[i] == 0)
					{
						fd_A[i] = new_fd;
						//每次新建立连接，设置该连接的超时数，如果此连接此后MAX_IDLECONNCTIME分钟内无反应，关闭该连接
						con_time[i] = MAX_IDLECONNCTIME;
						break;
					}
				}
				conn_amount++;

				printf("new connection client[%d] %s:%d\n", conn_amount,
						inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
				if(new_fd > maxsock)
					maxsock = new_fd;
			}else{
				printf("max connections arrive, exit\n");
				send(new_fd,"bye",4,0);
				close(new_fd);
				break;
			}
		}
		showclient();

	}

}
