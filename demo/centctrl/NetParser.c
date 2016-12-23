/********************************************************************
* $ID: NetParser.c          Thu 2016-09-22 17:09:42 +0800  lz       *
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
#include "public.h"

#define POLY              (0x8C)

int npto_CRC(void *src, int ilen)
{
	int i;
	unsigned char crc = 0, *ptr = (unsigned char*)src;

	for(i = 0 ; i < ilen; i++) {
		crc += ptr[i];
	}

	return crc;
}



int npto_Parse(void *src, int ilen, NetPtoPtr cmdPkt, int *olen, int *result)
{
	int i, cmdlen;
	unsigned char *ptr = (unsigned char * )src, checksum;

	int j;
	for(j =0 ; j< ilen; j++)
	{   
		printf(" %x", ptr[j]);  
	}   
	printf(" LEN=%d\n", ilen);

	for(i = 0; i< ilen; i++)
	{
		if(NPTO_MAGIC0 == ptr[i] && NPTO_MAGIC1 == ptr[i+1])
			break;
	}

	if(i >= ilen)
	{
		*olen = ilen;
		*result = NPTO_PARSE_RESULT_NOMAGIC;
		return -ilen;
	}

	cmdlen = ilen - i;
	if(cmdlen < NPTO_HEAD_SIZE)
	{
		*olen = i;
		*result = NPTO_PARSE_RESULT_TOSMALL;
		return 0;
	}

	memcpy(cmdPkt, &ptr[i], NPTO_HEAD_SIZE);
	if(cmdPkt->cmdlen > NPTO_MAX_SIZE)
	{
		*olen = i+2;
		*result = NPTO_PARSE_RESULT_UNKNOWN;

		return -7;
	}

	if(cmdPkt->cmdlen > cmdlen)
	{
		*olen = i;
		*result = NPTO_PARSE_RESULT_TOSMALL;
		return 0;
	}

	memcpy(cmdPkt->dat, &ptr[i+NPTO_HEAD_SIZE], cmdPkt->cmdlen-NPTO_HEAD_SIZE);
	checksum = cmdPkt->checksum;
	cmdPkt->checksum = 0;
	unsigned char  xx = npto_CRC(cmdPkt, cmdPkt->cmdlen);
	printf("cmd=%x CRC8=%x\n",cmdPkt->command,xx);
	if(checksum != npto_CRC(cmdPkt, cmdPkt->cmdlen)) {
		*olen = i + 2;
		*result = NPTO_PARSE_RESULT_ERRCRC;
		return -NPTO_PARSE_RESULT_ERRCRC;
	}

	*olen = i + cmdPkt->cmdlen;
	*result = NPTO_PARSE_RESULT_SUCC;
	return NPTO_PARSE_RESULT_SUCC;
}

int npto_GenerateRaw(void *obuf, int *olen, int cmd, unsigned int cmdseq,
		void *extdat, int extlen)
{
	int tmp;
	NetPtoPtr cmdPkt = (NetPtoPtr)obuf;

	cmdPkt->magic0    = NPTO_MAGIC0;
	cmdPkt->magic1    = NPTO_MAGIC1;
	cmdPkt->command   = cmd;
	cmdPkt->cmdseq    = cmdseq;

	memcpy(cmdPkt->dat, extdat, extlen);
/*	if((tmp =(extlen % 4)) > 0) {         // 命令长度为4的倍数
		tmp = 4 - tmp;
		memset(&cmdPkt->dat[extlen], 0, tmp);
		extlen += tmp;    
	}   */

	cmdPkt->cmdlen    = NPTO_HEAD_SIZE + extlen;
	cmdPkt->checksum  = 0;
	cmdPkt->checksum = npto_CRC(obuf,  cmdPkt->cmdlen);

	*olen = cmdPkt->cmdlen;
	return 0;
}



/******************* End Of File: NetParser.c *******************/
