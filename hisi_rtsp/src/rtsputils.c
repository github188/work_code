#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/time.h>
#include "fcntl.h"

#include "rtspservice.h"
#include "rtsputils.h"
#include "rtputils.h"
#include "ringfifo.h"
extern int g_s32DoPlay[CH_MAX];
//===============add===================

//=====================================

char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen, char *str, size_t len)
{
    switch(sa->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in  *sin = (struct sockaddr_in *) (void*)sa;

            if(inet_ntop(AF_INET, &sin->sin_addr, str, len) == NULL)
                return(NULL);
            return(str);
        }

        default:
            snprintf(str, len, "sock_ntop_host: unknown AF_xxx: %d, len %d",
                     sa->sa_family, salen);
            return(str);
    }
    return (NULL);
}

int tcp_accept(int fd)
{
    int f;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    memset(&addr,0,sizeof(addr));
    addrlen=sizeof(addr);

    /*接收连接，创建一个新的socket,返回其描述符*/
    f = accept(fd, (struct sockaddr *)&addr, &addrlen);

    return f;
}

void tcp_close(int s)
{
    close(s);
}

int tcp_connect(unsigned short port, char *addr)
{
    int f;
    int on=1;
    int one = 1;/*used to set SO_KEEPALIVE*/

    struct sockaddr_in s;
    int v = 1;
    if((f = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
    {
        fprintf(stderr, "socket() error in tcp_connect.\n");
        return -1;
    }
    setsockopt(f, SOL_SOCKET, SO_REUSEADDR, (char *) &v, sizeof(int));
    s.sin_family = AF_INET;
    s.sin_addr.s_addr = inet_addr(addr);//htonl(addr);
    s.sin_port = htons(port);
    // set to non-blocking
    if(ioctl(f, FIONBIO, &on) < 0)
    {
        fprintf(stderr,"ioctl() error in tcp_connect.\n");
        return -1;
    }
    if(connect(f,(struct sockaddr*)&s, sizeof(s)) < 0)
    {
        fprintf(stderr,"connect() error in tcp_connect.\n");
        return -1;
    }
    if(setsockopt(f, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one))<0)
    {
        fprintf(stderr,"setsockopt() SO_KEEPALIVE error in tcp_connect.\n");
        return -1;
    }
    return f;
}

int tcp_listen(unsigned short port)
{
    int f;
    int on=1;

    struct sockaddr_in s;
    int v = 1;

    if((f = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "socket() error in tcp_listen.\n");
        return -1;
    }

    setsockopt(f, SOL_SOCKET, SO_REUSEADDR, (char *) &v, sizeof(int));

    s.sin_family = AF_INET;
    s.sin_addr.s_addr = htonl(INADDR_ANY);
    s.sin_port = htons(port);


    if(bind(f, (struct sockaddr *)&s, sizeof(s)))
    {
        fprintf(stderr, "bind() error in tcp_listen");
        return -1;
    }

    if(ioctl(f, FIONBIO, &on) < 0)
    {
        fprintf(stderr, "ioctl() error in tcp_listen.\n");
        return -1;
    }


    if(listen(f, SOMAXCONN) < 0)
    {
        fprintf(stderr, "listen() error in tcp_listen.\n");
        return -1;
    }

    return f;
}

int tcp_read(int fd, void *buffer, int nbytes, struct sockaddr *Addr)
{
    int n;
    socklen_t Addrlen = sizeof(struct sockaddr);
    char addr_str[128];
    //fprintf(stderr, "readaddr:%s ", sock_ntop_host(Addr, Addrlen, addr_str, sizeof(addr_str)) );
    //fprintf(stderr, "readPort:%d\n",ntohs(((struct sockaddr_in *)Addr)->sin_port));
    n=recv(fd, buffer, nbytes, 0);
    //printf ("read count:%d\n",n);
    if(n>0)
    {

        if(getpeername(fd, Addr, &Addrlen) < 0)
        {
            fprintf(stderr,"error getperrname:%s %i\n", __FILE__, __LINE__);
        }
        else
        {

            fprintf(stderr, "%s ", sock_ntop_host(Addr, Addrlen, addr_str, sizeof(addr_str)));
            fprintf(stderr, "Port:%d\n",ntohs(((struct sockaddr_in *)(void*)Addr)->sin_port));
        }
    }

    return n;
}

/*
int tcp_write(int fd, void *buffer, int nbytes)
{
    int n;
    n = write(fd, buffer, nbytes);

    return n;
}
*/

int tcp_write(int connectSocketId, char *dataBuf, int dataSize)
{
    int     actDataSize;


    while(dataSize > 0)
    {
        actDataSize = send(connectSocketId, dataBuf, dataSize, 0);

        if(actDataSize<=0)
            break;

        dataBuf  += actDataSize;
        dataSize -= actDataSize;
    }

    if(dataSize > 0)
    {
        printf("Send Data error\n");
        return -1;
    }

    return 0;
}


stScheList sched[CH_MAX][MAX_CONNECTION];


int ScheduleInit()
{
    int i,j;
    pthread_t thread=0;
   int ch;

	for(j=0;j<CH_MAX;j++)
	{
		for(i=0; i<MAX_CONNECTION; ++i)
		{
			sched[j][i].rtp_session=NULL;
			sched[j][i].play_action=NULL;
			sched[j][i].valid=0;
			sched[j][i].BeginFrame=0;
		}
		ch=j;

		pthread_create(&thread,NULL,schedule_do,&ch);
		usleep(1000*500);
	}

    return 0;
}

void *schedule_do(void *arg)
{
    int i=0;
    struct timeval now;
    unsigned long long mnow;
    struct timespec ts = {0,33333};
    int s32FindNal = 0;
    int buflen=0,ringbuflen=0;
    struct ringbuf ringinfo;
#ifdef RTSP_DEBUG
    printf("The pthread %s start\n", __FUNCTION__);
#endif
    int firstI=0;
	int ch=*(int*)arg;

	printf("%s,%s,%d ch=%d\n",__FILE__,__FUNCTION__,__LINE__,ch);	

    do
    {
	     // for(ch=0;ch<CH_MAX;ch++)
	      {
			s32FindNal = 0;
			ringbuflen = ringget(ch,&ringinfo);	
			if(ringbuflen ==0)
			{
				nanosleep(&ts, NULL);
				continue ;
			}
			 
			s32FindNal = 1;
			for(i=0; i<MAX_CONNECTION; ++i)
			{
				if(sched[ch][i].valid)
				{
					if(!sched[ch][i].rtp_session->pause)
					{
						gettimeofday(&now,NULL);
						mnow = (now.tv_sec*1000 + now.tv_usec/1000);
						if((sched[ch][i].rtp_session->hndRtp)&&(s32FindNal))
						{
							buflen=ringbuflen;
							if(ringinfo.frame_type ==FRAME_TYPE_I)
								sched[ch][i].BeginFrame=1;
							if((firstI==0)&&(ringinfo.frame_type ==FRAME_TYPE_I))
							{
								firstI=1;
							}
							if(firstI)
								sched[ch][i].play_action((unsigned int)(sched[ch][i].rtp_session->hndRtp), (char *)ringinfo.buffer, ringinfo.size, mnow);
						}
					}
				}
			}

		  }
    }
	while(1);

//cleanup:

    //free(pDataBuf);
    //close(s32FileId);

#ifdef RTSP_DEBUG
    printf("The pthread %s end\n\n\n\n", __FUNCTION__);
#endif
    return ERR_NOERROR;
}


int schedule_add(int ch,RTP_session *rtp_session)
{
    int i;
    for(i=0; i<MAX_CONNECTION; ++i)
    {
        if(!sched[ch][i].valid)
        {
            sched[ch][i].valid=1;
            sched[ch][i].rtp_session=rtp_session;

            sched[ch][i].play_action=RtpSend;
            printf("**adding a schedule object action %s,%d**\n", __FILE__, __LINE__);
            return i;
        }
    }
    return ERR_GENERIC;
}

int schedule_start(int ch,int id,stPlayArgs *args)
{

    sched[ch][id].rtp_session->pause=0;
    sched[ch][id].rtp_session->started=1;


    return ERR_NOERROR;
}

void schedule_stop(int id)
{
//    RTCP_send_packet(sched[id].rtp_session,SR);
//    RTCP_send_packet(sched[id].rtp_session,BYE);
}

int schedule_remove(int ch,int id)
{
    sched[ch][id].valid=0;
    sched[ch][id].BeginFrame=0;
    return ERR_NOERROR;
}



int bwrite(char *buffer, unsigned short len, RTSP_buffer * rtsp)
{
    if((rtsp->out_size + len) > (int) sizeof(rtsp->out_buffer))
    {
        fprintf(stderr,"bwrite(): not enough free space in out message buffer.\n");
        return ERR_ALLOC;
    }
    memcpy(&(rtsp->out_buffer[rtsp->out_size]), buffer, len);
    rtsp->out_buffer[rtsp->out_size + len] = '\0';
    rtsp->out_size += len;

    return ERR_NOERROR;
}

int send_reply(int err, char *addon, RTSP_buffer * rtsp)
{
    unsigned int len;
    char *b;
    int res;

    if(addon != NULL)
    {
        len = 256 + strlen(addon);
    }
    else
    {
        len = 256;
    }

    b = (char *) malloc(len);
    if(b == NULL)
    {
        fprintf(stderr,"send_reply(): memory allocation error.\n");
        return ERR_ALLOC;
    }
    memset(b, 0, sizeof(b));
    sprintf(b, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL, RTSP_VER, err, get_stat(err), rtsp->rtsp_cseq);
    strcat(b, RTSP_EL);

    res = bwrite(b, (unsigned short) strlen(b), rtsp);
    free(b);

    return res;
}


const char *get_stat(int err)
{
    struct
    {
        const char *token;
        int code;
    } status[] =
    {
        {
            "Continue", 100
        }, {
            "OK", 200
        }, {
            "Created", 201
        }, {
            "Accepted", 202
        }, {
            "Non-Authoritative Information", 203
        }, {
            "No Content", 204
        }, {
            "Reset Content", 205
        }, {
            "Partial Content", 206
        }, {
            "Multiple Choices", 300
        }, {
            "Moved Permanently", 301
        }, {
            "Moved Temporarily", 302
        }, {
            "Bad Request", 400
        }, {
            "Unauthorized", 401
        }, {
            "Payment Required", 402
        }, {
            "Forbidden", 403
        }, {
            "Not Found", 404
        }, {
            "Method Not Allowed", 405
        }, {
            "Not Acceptable", 406
        }, {
            "Proxy Authentication Required", 407
        }, {
            "Request Time-out", 408
        }, {
            "Conflict", 409
        }, {
            "Gone", 410
        }, {
            "Length Required", 411
        }, {
            "Precondition Failed", 412
        }, {
            "Request Entity Too Large", 413
        }, {
            "Request-URI Too Large", 414
        }, {
            "Unsupported Media Type", 415
        }, {
            "Bad Extension", 420
        }, {
            "Invalid Parameter", 450
        }, {
            "Parameter Not Understood", 451
        }, {
            "Conference Not Found", 452
        }, {
            "Not Enough Bandwidth", 453
        }, {
            "Session Not Found", 454
        }, {
            "Method Not Valid In This State", 455
        }, {
            "Header Field Not Valid for Resource", 456
        }, {
            "Invalid Range", 457
        }, {
            "Parameter Is Read-Only", 458
        }, {
            "Unsupported transport", 461
        }, {
            "Internal Server Error", 500
        }, {
            "Not Implemented", 501
        }, {
            "Bad Gateway", 502
        }, {
            "Service Unavailable", 503
        }, {
            "Gateway Time-out", 504
        }, {
            "RTSP Version Not Supported", 505
        }, {
            "Option not supported", 551
        }, {
            "Extended Error:", 911
        }, {
            NULL, -1
        }
    };

    int i;
    for(i = 0; status[i].code != err && status[i].code != -1; ++i);

    return status[i].token;
}
