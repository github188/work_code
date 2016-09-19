/********************************************************************
* $ID: rs232.c    二, 04 10月 2011 22:29:31 +0800  feijianhf        *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  王刚(danny.wang)  <danny.wang@besovideo.com>         *
*                                                                   *
* CopyRight (c)  2011  besovideo                                    *
*   All rights reserved.                                            *
*                                                                   *
* This file is free software;                                       *
*   you are free to modify and/or redistribute it                   *
*   under the terms of the GNU General Public Licence (GPL).        *
*                                                                   *
* Last modified: 四, 06 10月 2011 15:23:27 +0800       by jw #
*                                                                   *
* No warranty, no liability, use this at your own risk!             *
********************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "rs232.h" 


/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
 */
const int speed_arr[] = {
    B4000000, B3500000, B3000000, B2500000, B2000000, B1500000, B1152000, B1000000,
    B921600,  B576000,  B500000,  B460800,  B230400,  B115200,
    B57600,   B38400,   B19200, 
    B9600,    B4800,    B2400,    B1800,    B1200,    B600,     B300, B19200};
const int name_arr[] = {
    4000000, 3500000, 3000000, 2500000, 2000000, 1500000, 1152000, 1000000,
    921600,  576000,  500000,  460800,  230400,  115200,
    57600,   38400,   19200, 
    9600,    4800,    2400,    1800,    1200,    600,     300, 19200};
static int set_speed(int fd, int speed)
{
    int   i; 
    int   status; 
    struct termios   Opt;
    tcgetattr(fd, &Opt); 
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
	if(speed == name_arr[i]) {     
	    tcflush(fd, TCIOFLUSH);     
	    cfsetispeed(&Opt, speed_arr[i]);  
	    cfsetospeed(&Opt, speed_arr[i]);   
	    status = tcsetattr(fd, TCSANOW, &Opt);  
	    if  (status != 0) {        
		perror("tcsetattr fd1");  
		return -1;
	    }    
	    tcflush(fd,TCIOFLUSH);   
	}  
    }

    return 0;
}

static long GetBaudRate(long baudRate)
{
    int i;
    long BaudR = B19200;

    for ( i= 0;  i < sizeof(name_arr) / sizeof(int);  i++)  {
	if(baudRate == name_arr[i]) {
	    BaudR = speed_arr[i];
	    break;
	}
    }
    return BaudR;
}



/**
 *@brief   设置串口数据位，停止位和效验位
 *@param  fd     类型  int  打开的串口文件句柄
 *@param  databits 类型  int 数据位   取值 为 7 或者8
 *@param  stopbits 类型  int 停止位   取值为 1 或者2
 *@param  parity  类型  int  效验类型 取值为N,E,O,,S
 */
static int set_Parity(int fd,int databits,int stopbits,int parity, const char *deviceName)
{ 
    struct termios options; 

    if(tcgetattr( fd, &options)  !=  0) { 
	printf("GetSerial Attr:%s\n", deviceName);     
	return (-1);  
    }

    options.c_cflag &= ~CSIZE; 
    switch (databits) { /*设置数据位数*/
	case 7:	
	    options.c_cflag |= CS7; 
	    break;
	case 8:     
	    options.c_cflag |= CS8;
	    break;   
	default:    
	    fprintf(stderr,"Unsupported data size\n"); 
	    return -2;
    }

    switch (parity)  {
	case 'n':
	case 'N':    
	    options.c_cflag &= ~PARENB;   /* Clear parity enable */
	    options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
	    break;  
	case 'o':   
	case 'O':     
	    options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
	    options.c_iflag |= INPCK;             /* Disnable parity checking */ 
	    break;  
	case 'e':  
	case 'E':   
	    options.c_cflag |= PARENB;     /* Enable parity */    
	    options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
	    options.c_iflag |= INPCK;       /* Disnable parity checking */
	    break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
	    options.c_cflag &= ~CSTOPB;break;  
	default:   
	    fprintf(stderr,"Unsupported parity\n");    
	    return (-3);  
    } 

    /* 设置停止位*/  
    switch (stopbits)
    {   
	case 1:    
	    options.c_cflag &= ~CSTOPB;  
	    break;  
	case 2:    
	    options.c_cflag |= CSTOPB;  
	    break;
	default:    
	    fprintf(stderr,"Unsupported stop bits\n");  
	    return (-4); 
    } 

    /* Set input parity option */ 
    if (parity != 'n')   
	options.c_iflag |= INPCK; 
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/   
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */

    if (tcsetattr(fd,TCSANOW,&options) != 0) {   
	printf("SetupSerial: %s", deviceName);   
	return (5);  
    } 

    return 0;  
}

/*
 ** Function: OpenCom
 **
 ** Description:
 **    Opens a serial port with the specified parameters
 **
 ** Arguments:
 **    portNo - handle used for further access
 **    deviceName - the name of the device to open
 **    baudRate - rate to open (57600 for example)
 **    parity - 0 for no parity
 **    dataBits - 7 or 8
 **    stopBits - 1 or 2
 **    iqSize - ignored
 **    oqSize - ignored
 **
 ** Returns:
 **    -1 on failure
 **
 ** Limitations:
 **    parity, stopBits, iqSize, and oqSize are ignored
 */
int OpenCom( const char *deviceName, long baudRate, int parity,
    int dataBits, int stopBits, int flow, int iqSize, int oqSize)
{
    int handle;
    struct termios newtio;

    if(!deviceName) return -1;

    if ((handle = open(deviceName, O_RDWR)) == -1) {
	printf("Error Open device:%s\n", deviceName);
	return -2;
    }

    /* set the port to raw I/O */
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_iflag = 0/*IGNPAR*/;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VINTR]    = 0;
    newtio.c_cc[VQUIT]    = 0;
    newtio.c_cc[VERASE]   = 0;
    newtio.c_cc[VKILL]    = 0;
    newtio.c_cc[VEOF]     = 4;
    newtio.c_cc[VTIME]    = 0;
    newtio.c_cc[VMIN]     = 1;
    newtio.c_cc[VSWTC]    = 0;
    newtio.c_cc[VSTART]   = 0;
    newtio.c_cc[VSTOP]    = 0;
    newtio.c_cc[VSUSP]    = 0;
    newtio.c_cc[VEOL]     = 0;
    newtio.c_cc[VREPRINT] = 0;
    newtio.c_cc[VDISCARD] = 0;
    newtio.c_cc[VWERASE]  = 0;
    newtio.c_cc[VLNEXT]   = 0;
    newtio.c_cc[VEOL2]    = 0;

    if(flow) newtio.c_cflag |= CRTSCTS;

    cfsetospeed(&newtio, GetBaudRate(baudRate));
    cfsetispeed(&newtio, GetBaudRate(baudRate));
    tcsetattr(handle, TCSANOW, &newtio);

    set_speed(handle, baudRate);
    if(set_Parity(handle, dataBits, stopBits, parity, deviceName) < 0) {
	CloseCom(handle);
	return -5;
    }

    return handle;
}

/*
 ** Function: CloseCom
 **
 ** Description:
 **    Closes a previously opened port
 **
 ** Arguments:
 **    The port handle to close
 **
 **    Returns:
 **    -1 on failure
 */
int CloseCom(int handle)
{
    if(handle > 0) {
	close(handle), handle = 0;
	return 0;
    }

    return -1;
}



/********************* End Of File: rs232.c *********************/
