/********************************************************************
* $ID: rtputils.h           Thu 2016-09-29 17:36:04 +0800  lz       *
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
#ifndef  __RTPUTILS_DEF_H
#define  __RTPUTILS_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RTP_PKT_LENGTH     1400

#define H264                    96
#define G711                    97

	typedef enum
	{
		_h264       = 0x100,
		_h264nalu,
		_mjpeg,
		_g711       = 0x200,
	}EmRtpPayload;

	unsigned int RtpCreate(unsigned int u32IP, int s32Port, EmRtpPayload emPayload);
	void RtpDelete(unsigned int u32Rtp);
	unsigned int RtpSend(unsigned int u32Rtp, char *pData, int s32DataSize, unsigned int u32TimeStamp);

#ifdef __cplusplus
}
#endif



#endif// __RTPUTILS_DEF_H
/******************* End Of File: rtputils.h *******************/
