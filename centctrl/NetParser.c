/********************************************************************
* $ID: NetParser.c   Mon 2013-08-05 19:22:18 +0800  dannywang       *
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

#define POLY        (0x8C)

int npto_CRC(void *src, int ilen)
{	
    int i;
    unsigned char crc = 0, *ptr = (unsigned char*)src;

    for(i = 0 ; i < ilen; i++) {
	crc += ptr[i];
    }

    return crc;
}

//CRC-8校验
unsigned char CRC8(unsigned char *ptr, unsigned short len)
{
    unsigned char crc = 0;
    unsigned char i;
    while(len--) {
        crc ^=*ptr++;
        for(i = 0; i < 8; i++) {
            if(crc & 0x01) {
                crc = (crc >> 1) ^ POLY;
            }else crc >>= 1;
        }   
    }   
    return crc;
}

// 从当前缓冲区中查找一条完整的命令
// @src,ilen   查找缓冲区地址及大小
// @cmdPkt     完整命令存取地址
// @olen       缓冲区使用大小，注意它可能会大于当前命令的长度
// @result     执行结果
int npto_Parse(void *src, int ilen, NetPtoPtr cmdPkt, int *olen, int *result)
{
    int i, cmdlen;
    unsigned char *ptr = (unsigned char*)src, checksum;
	
	int j;
	for(j =0 ; j< ilen; j++)
	{
		printf(" %x", ptr[j]);	
	}
	printf(" LEN=%d\n", ilen);
	
    // 查找协议头
    for(i = 0; i < ilen; i++) {
	if(NPTO_MAGIC0 == ptr[i] && NPTO_MAGIC1 == ptr[i+1])
	    break;
    }
    if(i >= ilen) {
	// 没有找到协议头
	*olen = ilen;
	*result = NPTO_PARSE_RESULT_NOMAGIC;
	return -ilen;       // 将所有的数据都设置为使用
    }

    cmdlen = ilen - i;
    if(cmdlen < NPTO_HEAD_SIZE) {       // too small data
	*olen = i;
	*result = NPTO_PARSE_RESULT_TOSMALL;
	return 0;
    }

    memcpy(cmdPkt, &ptr[i], NPTO_HEAD_SIZE);
	if(cmdPkt->cmdlen > NPTO_MAX_SIZE) {  // Error data
	*olen = i + 2;
       *result = NPTO_PARSE_RESULT_UNKNOWN;
	return -7;
    }

    if(cmdPkt->cmdlen > cmdlen) {
	*olen = i;
	*result = NPTO_PARSE_RESULT_TOSMALL;
	return 0;
    }

    memcpy(cmdPkt->dat, &ptr[i+NPTO_HEAD_SIZE], cmdPkt->cmdlen-NPTO_HEAD_SIZE);
    checksum = cmdPkt->checksum;
    cmdPkt->checksum = 0;         // set zero for recalc
    if(checksum != CRC8(cmdPkt, cmdPkt->cmdlen)) {
	*olen = i + 2;
	*result = NPTO_PARSE_RESULT_ERRCRC;
	return -NPTO_PARSE_RESULT_ERRCRC;
    }

    *olen = i + cmdPkt->cmdlen;
    *result = NPTO_PARSE_RESULT_SUCC;
    return NPTO_PARSE_RESULT_SUCC;
}

// 生成一条命令序列
// @obuf,olen  输出缓冲区地址及大小，同时也返回最后组成命令的长度
// @cmdcode    命令名称
// @cmdseq     当前命令的序号
// @extdat,extlen   当前命令的载体数据及长度
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
    if((tmp =(extlen % 4)) > 0) {         // 命令长度为4的倍数
        tmp = 4 - tmp;
	memset(&cmdPkt->dat[extlen], 0, tmp);
	extlen += tmp;		
    }

    cmdPkt->cmdlen    = NPTO_HEAD_SIZE + extlen;
    cmdPkt->checksum  = 0;
    cmdPkt->checksum = CRC8(obuf,  cmdPkt->cmdlen);

    *olen = cmdPkt->cmdlen;
    return 0;
}

// 生成一条反馈命令
int npto_GenerateResp(void *obuf, int *olen, NetPtoPtr cmdPkt, int result, 
        void *extdat, int extlen)
{
    unsigned char buffer[NPTO_MAX_SIZE];

    buffer[0] = (unsigned char) result;
    if (extdat) {
        memcpy(&buffer[1], extdat, extlen);
        extlen += 1;
    }else 
        extlen = 1;
    
    return npto_GenerateRaw(obuf, olen, cmdPkt->command | 0x80, \
            cmdPkt->cmdseq, buffer, extlen);
}



/******************* End Of File: NetParser.c *******************/
