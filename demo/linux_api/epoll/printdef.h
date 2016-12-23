/********************************************************************
* $ID: printdef.h          Fri 2010-04-16 10:21:02 +0800  whg       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <danny.wang@besovideo.com>             *
*                                                                   *
* CopyRight (c) 2010 BESOVIDEO                                      *
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
#ifndef  __PRINTDEF_DEF_H
#define  __PRINTDEF_DEF_H

#define PRINT_TIME               // line start time
#define PRINT_ERROR             // error message
#define PRINT_FUNC              // func entry
#define PRINT_DEBUG              // debug message 
#define PRINT_MESSG             // print message for user success
#define PRINT_ASSERT            // test some is not null

#define PRINT_PRONAME          "[epoll] "

enum {PLOG_NONE=0x00, PLOG_ERROR=0x01, PLOG_TIMER=0x02, PLOG_MESSAGE=0x04, PLOG_FUNC=0x08, 
    PLOG_PROTO_N=0x10, PLOG_PROTO_B=0x20, PLOG_TEMP=0x80, PLOG_DEBUG=0x80, PLOG_BATDATA=0x100, };
extern int g_log_level;

#ifdef PRINT_TIME
#include <time.h>
#include <sys/time.h>
  #define DATA_TIME_NOW(fmt, args...)                                          \
    do{                                                                        \
	struct timeval _tNow; struct tm _tmNow;                                \
	gettimeofday(&_tNow, NULL);                                            \
	memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));             \
	printf("[%02d %02d:%02d:%02d.%03ld] ",                                 \
	    _tmNow.tm_mday,                                                    \
	    _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec, _tNow.tv_usec/1000); \
    }while(0)
#else
  #define DATA_TIME_NOW(fmt, args...)
#endif                        // PRINT_TIME

#define PVERSION(fmt, args...)                                                 \
    do{	printf("\033[01;31;31m");                                              \
	printf(fmt, ##args);                                                   \
	printf(" Build time: %s, %s.\n\033[0m", __DATE__, __TIME__);           \
    }while(0)

#ifdef PRINT_ERROR
  #define PERROR(fmt, args...)                                                 \
    do{	if(!(g_log_level & PLOG_ERROR)) break;                                 \
	if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf("\033[01;31;31m");                                              \
	printf(PRINT_PRONAME"  **%s():%4d> ", __FUNCTION__, __LINE__);         \
	printf(fmt, ##args);                                                   \
	printf("\033[0m");                                                     \
    }while(0)
#else
  #define PERROR(fmt, args...)
#endif				// PRINT_ERROR

#ifdef PRINT_FUNC
  #define PFUNC(fmt, args...)                                                  \
    do{ if(!(g_log_level & PLOG_FUNC)) break;                                  \
	if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf(PRINT_PRONAME"  %s():%4d> ", __FUNCTION__, __LINE__);           \
	printf(fmt, ##args);                                                   \
    }while(0)

  #define PNETR(fmt, args...)                                                  \
    do{ if(!(g_log_level & PLOG_PROTO_N)) break;                               \
	if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf("\033[00;32;40m");                                              \
	printf(PRINT_PRONAME"  %s():%4d> ", __FUNCTION__, __LINE__);           \
	printf(fmt, ##args);                                                   \
	printf("\033[0m");                                                     \
    }while(0)

  #define PNETS(fmt, args...)                                                  \
    do{ if(!(g_log_level & PLOG_PROTO_N)) break;                               \
	if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf("\033[01;32;40m");                                              \
	printf(PRINT_PRONAME"  %s():%4d> ", __FUNCTION__, __LINE__);           \
	printf(fmt, ##args);                                                   \
	printf("\033[0m");                                                     \
    }while(0)
  #define PBATR(fmt, args...)                                                  \
    do{ if(!(g_log_level & PLOG_PROTO_B)) break;                               \
	if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf("\033[00;35;40m");                                              \
	printf(PRINT_PRONAME"  %s():%4d> ", __FUNCTION__, __LINE__);           \
	printf(fmt, ##args);                                                   \
	printf("\033[0m");                                                     \
    }while(0)
  #define PBATS(fmt, args...)                                                  \
    do{ if(!(g_log_level & PLOG_PROTO_B)) break;                               \
	if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf("\033[01;35;40m");                                              \
	printf(PRINT_PRONAME"  %s():%4d> ", __FUNCTION__, __LINE__);           \
	printf(fmt, ##args);                                                   \
	printf("\033[0m");                                                     \
    }while(0)


#else
  #define PFUNC(fmt, args...)
  #define PNET(fmt, args...)
  #define PNES(fmt, args...)
  #define PBATS(fmt, args...)
  #define PBATR(fmt, args...)
#endif				// PRINT_FUNC

#ifdef PRINT_DEBUG
  #define PDEBUG(fmt, args...)                                                 \
    do{ if(!(g_log_level & PLOG_DEBUG)) break;                                 \
        if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf(PRINT_PRONAME"DEBUG> ");                                        \
	printf(fmt, ##args);                                                   \
    }while(0)
#else
  #define PDEBUG(fmt, args...)
#endif				// PRINT_DEBUG

#ifdef PRINT_MESSG
  #define PMSG(fmt, args...)                                                   \
    do{ if(!(g_log_level & PLOG_MESSAGE)) break;                               \
        if(g_log_level & PLOG_TIMER) DATA_TIME_NOW();                          \
	printf(PRINT_PRONAME" %s:%d ", __FILE__, __LINE__);                    \
	printf(fmt, ##args);                                                   \
    }while(0)
#else
  #define PMSG(fmt, args...)
#endif				// PRINT_MESSG

#ifdef PRINT_ASSERT
  #define ASSERT(a)                                                            \
    do{ if(!(a)){ printf(PRINT_PRONAME"<%s>:%d, assert(false)\n", __FILE__, __LINE__);      \
	while(1);}                                                             \
    }while(0)
#else
  #define ASSERT(a)
#endif				// PRINT_ASSERT

#if 0
Text   attributes
0   All   attributes   off  
1   Bold   on  
4   Underscore   (on   monochrome   display   adapter   only)  
5   Blink   on  
7   Reverse   video   on  
8   Concealed   on  

Foreground   colors  
30   Black  
31   Red  
32   Green  
33   Yellow  
34   Blue  
35   Magenta  
36   Cyan  
37   White  

Background   colors  
40   Black  
41   Red  
42   Green  
43   Yellow  
44   Blue  
45   Magenta  
46   Cyan  
47     White   
#endif

#endif// __PRINTDEF_DEF_H
/******************* End Of File: printdef.h *******************/
