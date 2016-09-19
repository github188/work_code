/********************************************************************
* $ID: param.c       Thu 2013-10-24 14:05:49 +0800  dannywang       *
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "public.h"
#include "printdef.h"


#define MAX_PARAM_LEN         (8192)

// 192.168.1.22 -->C0 A8 01 16
int prm_netip_2_binary(unsigned char *dst, const char *src)
{
    int i, cnt = 4;;
    char *ptr = (char*)src;

    if(!dst || !src) return -1;

    for(i = 0; i < cnt; i++) {
	dst[i] = (unsigned char)strtol(ptr, (char**)NULL, 10);

	if((ptr = index(ptr, '.'))) ptr += 1;
	else break;
    }

    return (i >= cnt) ? 0 : -2;
}

// aa:bb:cc:dd:dd:ee  -->AA BB CC DD DD EE
int prm_netmac_2_binary(unsigned char *dst, const char *src)
{
    int i, cnt = 6;;
    char *ptr = (char*)src;

    if(!dst || !src) return -1;

    for(i = 0; i < cnt; i++) {
	dst[i] = (unsigned char)strtol(ptr, (char**)NULL, 16);

	if((ptr = index(ptr, ':'))) ptr += 1;
	else break;
    }

    return (i >= cnt) ? 0 : -2;
}

const int prm_Default_fromENV(const char *env, char *obuf)
{
    char *ptr;

    ASSERT(env);
    ptr = getenv(env);

    if(!strncasecmp(env, "mac", 3)) {
	if(!ptr) ptr = "00:00:22:22:22:22";
	snprintf(obuf, 18+1, "%s", ptr);
    }
    else if(!strncasecmp(env, "hwver", 5)) {
	if(!ptr) ptr = "V1.01";
	snprintf(obuf, 9, "%s", ptr);
    }
    else if(!strncasecmp(env, "hwdate", 6)) {
	if(!ptr) ptr = "20121212";
	snprintf(obuf, 9, "%s", ptr);
    }
    else snprintf(obuf, 64, "%s", ptr);

    return 0;
}

// set all param default
int prm_Default(ParamInfo *pinfo)
{
    if(!pinfo) return -1;

    memset(pinfo, 0, sizeof(ParamInfo));

    pinfo->listport  = DEF_PORT;
    snprintf(pinfo->net_ip, sizeof(pinfo->net_ip), "%s", DEF_IP);
    snprintf(pinfo->net_mac, sizeof(pinfo->net_mac), "%s", "00:0c:e9:b5:9a:06");
    snprintf(pinfo->sn, sizeof(pinfo->sn), "%s", SER_NUM);
    return 0;
}


//------------------------------------------------------------------
//         SAVE AND LOAD
//------------------------------------------------------------------
int flashcp_memory_write(const char *devicename, int devoffset,
	const char *bufFlag, int bufFlagsize, const char *inBuf, int iSize);
static int prm_SaveFlash(void *buf, int len, const char *devname)
{
    int fd;

#ifndef __i386__
    if(0 == strncmp(devname, "/dev/mtd", 8))
	return flashcp_memory_write(devname, 0, buf, len, NULL, 0);
#endif

    if((fd = open(devname, O_RDWR|O_CREAT|O_TRUNC, 0766)) < 0) {
	PERROR("Error Open:%s\n", devname);
	return -2;
    }

    write(fd, buf, len);
    close(fd);

    return 0;
}

// ret: data count for read, 
//      <0: error
static int prm_LoadFlash(void *obuf, int len, const char *devname)
{
    int fd, ret = 0;

    // read file
    if((fd = open(devname, O_RDONLY)) < 0) {
	PERROR("Error Open file:%s\n", devname);
	ret = -4;
	goto out;
    }
    if((ret = read(fd, obuf, len)) < 0) {
	PERROR("Error Read file:%s\n", devname);
	ret = -5;
	goto out;
    }

out:
    close(fd), fd = -1;
    return ret;
}


// save param to file
int prm_Save(ParamInfo *pinfo, const char *fname)
{
    int count = MAX_PARAM_LEN, ret = 0;
    char *ptr, *data = NULL;
    char *plen, lenbuf[24];

    if(!(data = (char*)malloc(count))) {
	PERROR("Out of memory, Malloc size:%d\n", count);
	ret = -2;
	goto out;
    }
    memset(data, 0, count);
    ptr = data;

    // first ParamInfo Sect for save length
    ptr += sprintf(ptr, "[%s]\n","SETTINGS");
    plen = ptr;
    ptr += sprintf(ptr, "paramlen=%06d\n", 0);
    ptr += sprintf(ptr, "port=%04d\n", pinfo->listport);
    ptr += sprintf(ptr, "net_myip=%s\n", pinfo->net_ip);
    ptr += sprintf(ptr, "net_mac=%s\n", pinfo->net_mac);
    ptr += sprintf(ptr, "serialnum=%s\n", pinfo->sn);

    // set file count
    sprintf(lenbuf, "paramlen=%06d\n", (ptr-data));
    memcpy(plen, lenbuf, strlen(lenbuf));

    ASSERT((ptr-data) < count);

    // write moer 4 byte, this is zeor
    // on read, this is string end
    prm_SaveFlash(data, ptr-data+4, fname);

out:
    if(data) free(data), data = NULL;

    return ret;
}

// hw/sw version
static int prm_Load_SETTINGS(ParamInfo *pinfo, const char *sline, void *dat)
{
    char *ptr = index(sline, '=');

    if(!ptr) return -2;

    ptr += 1;
    if(!strncmp(sline, "paramlen", 8))
	PFUNC("Param Len:%d\n", atoi(ptr));
    else if(!strncmp(sline, "port", 4))
	pinfo->listport = atoi(ptr);
    else if(!strncmp(sline, "myip", 4))
	sprintf((char*)pinfo->net_ip, "%s", ptr);
    else if(!strncmp(sline, "serialnum", 9))
	sprintf((char*)pinfo->sn, "%s", ptr);
    else if(!strncmp(sline, "mac", 4))
	sprintf((char*)pinfo->net_mac, "%s", ptr);
    else return -2;

    return 0;
}

int prm_Load(ParamInfo *pinfo, const char *fname)
{
    int count = MAX_PARAM_LEN, loadok = 0;
    int ret = -1;
    char *p, *ptr, *data = NULL, sline[256];
    int (*load_sect)(ParamInfo *pinfo, const char *sline, void*dat) = NULL;

    if(!(data = (char*)malloc(count))) {
	PERROR("Out of memory, Malloc size:%d\n", count);
	ret = -2;
	goto out;
    }
    memset(data, 0, count);
    ptr = data;

    // read and check
    if(prm_LoadFlash(data, count, fname) < 0) return -3;
    if(0xFF == (unsigned char)data[0]) return -5;

    data[count-1] = 0;
    while(1) {
	if(!(p = index(ptr, '\n'))) break;

	if(ptr[0] == '[') {      // sect start
	    snprintf(sline, p-ptr-1, "%s", &ptr[1]);
	    if(!strcmp(sline, "SETTINGS"))       load_sect = &prm_Load_SETTINGS;
	    else 	load_sect = NULL;
	}
	else if(load_sect) {
	    snprintf(sline, p-ptr+1, "%s", ptr);
	    if(load_sect(pinfo, sline, NULL) == 0)
		loadok = 1;
	}
	else ; // check save or error

	ptr = p + 1;
    }

    if(loadok) ret = 0;

    // secute 

out:
    if(data) free(data), data = NULL;
    return ret;
}

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
int check_local_ip(const char *ipaddr)
{
    int ret = -1, a, b, c, d;
    int ip[4];
    char addressBuffer[INET_ADDRSTRLEN];
    void * tmpAddrPtr=NULL;
    struct ifaddrs * ifAddrStruct=NULL;

    sscanf(addressBuffer, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);

    if(getifaddrs(&ifAddrStruct) < 0) return ret;

    while (ifAddrStruct) {
	if (ifAddrStruct->ifa_addr && ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
	    // is a valid IP4 Address
	    tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
	    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

//	    PFUNC("NetIp, Address [%s]%s\n", ifAddrStruct->ifa_name, addressBuffer);
	    if(sscanf(addressBuffer, "%d.%d.%d.%d", &a, &b, &c, &d) == 4) {
		if(a == ip[0] && b == ip[1] && c == ip[2] && d == ip[3]) {
		    PFUNC("Same IP Address %s->%s, no need setting\n", ifAddrStruct->ifa_name, addressBuffer);
		    ret = 0;
		    break;
		}
	    }
	}
	ifAddrStruct=ifAddrStruct->ifa_next;
    }

    return ret;
}

extern char g_rs232_dev[];
// Set Board info by param set
int prm_SetBoard(ParamInfo *pinfo)
{
#ifndef __i386__
    char command[256];
//    char addr[32];
    int fd;//, gpioVal = 0;

    // check runing nfs
    if((fd = open("/proc/cmdline",  O_RDONLY)) > 0) {
	read(fd, command, 256);
	close(fd);

	if(strstr(command, "ttymxc0")) {
	    sprintf(g_rs232_dev, "/dev/%s", "ttymxc1");
	}
	else
	    sprintf(g_rs232_dev, "/dev/%s", "tty0");

	if(strstr(command, "nfsroot")) {
	    printf("Run NFS\n");
	    return 0;
	}
    }

#if 0
    gpioVal = misc_read_serialGPIO();
    mac_addr_get(addr, pinfo->net_ip, gpioVal);

    if(check_local_ip(addr) == 0)
	return 0;

    // network down
    sprintf(command, "/sbin/ifconfig eth0 %s", "down"); system(command);

    // mac
    mac_addr_get(addr, pinfo->net_mac, gpioVal);
    sprintf(command, "/sbin/ifconfig eth0 hw ether %s", addr);
    system(command);

    mac_addr_get(addr, pinfo->net_ip, gpioVal);
    sprintf(command, "/sbin/ifconfig eth0 %s netmask 255.255.255.0 up",
	    addr);
    system(command);
#endif

#endif
    return 0;
}

/********************* End Of File: param.c *********************/
