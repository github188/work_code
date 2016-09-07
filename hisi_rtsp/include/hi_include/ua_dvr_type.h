#ifndef __H264__ENCODE__LIB__TYPE__H__
#define __H264__ENCODE__LIB__TYPE__H__
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/videodev.h>
#include <errno.h>
#include <linux/fb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/un.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/mount.h>
#include <sys/vfs.h>
#include <sys/msg.h>
#include <termios.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <dirent.h>
#include <net/route.h>
#include<netpacket/packet.h>
#include<linux/if_ether.h>
#include <linux/rtc.h>
#include<ctype.h>
#include <linux/soundcard.h>




typedef enum
{
	HI_FALSE = 0,
	true  = 1
} bool;

#define   SOCKADDR_IN       struct sockaddr_in
#define   SOCKADDR          struct sockaddr
#define   SOCKET            int
#define SOCKET_ERROR  -1


#define DVR_EXT_DEBUG 1
#if DVR_EXT_DEBUG
#define DVR_DEBUG(msg...)    printf("[ %s,%s, %d ]=>",__FILE__, __FUNCTION__, __LINE__);  printf(msg);printf("\r\n");
#define DVR_ERROR(msg...)     fprintf(stderr, "[ error: ====>%s, %d]=>",__FILE__,  __LINE__);printf(msg); printf("\r\n")
#define DVR_ASSERT(exp)      \
                        do {\
                                if (!(exp)) {\
                                printf( "[ %s ]=> ",#exp);printf("\r\n");\
                                assert(exp);\
                                }\
                        } while (0)

#else
#define DVR_DEBUG(msg...)
#define DVR_ERROR(msg...)
#define DVR_ASSERT(exp...)
#endif


#define	DVR_VIDEO_MAX_FRAME_SIZE		(40)
#define DVR_VIDEO_MAX_UNITE_SIZE     (1024*120)
#define	DVR_AUDIO_MAX_FRAME_SIZE		(32)
#define	DVR_AUDIO_MAX_UNITE_SIZE		(1024*4)

#define DVR_CHANNEL_NUMS  8



#endif	// __H264__ENCODE__LIB__TYPE__H__

