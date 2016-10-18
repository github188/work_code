/********************************************************************
* $ID: rtspservice.h        Thu 2016-09-29 17:37:34 +0800  lz       *
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
#ifndef  __RTSPSERVICE_DEF_H
#define  __RTSPSERVICE_DEF_H


#include "rtsputils.h"

#define RTSP_DEBUG 1
#define RTP_DEFAULT_PORT 5004

void CallBackNotifyRtspExit(char s8IsExit);
void *ThreadRtsp(void *pArgs);
int rtsp_server(RTSP_buffer *rtsp);
void IntHandl(int i);
void UpdateSps(int ch,unsigned char *data,int len);
void UpdatePps(int ch,unsigned char *data,int len);


#endif// __RTSPSERVICE_DEF_H
/****************** End Of File: rtspservice.h ******************/
