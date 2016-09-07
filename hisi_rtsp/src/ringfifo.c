/*ringbuf .c*/

#include<stdio.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ringfifo.h"
#include "rtputils.h"
#include "sample_comm.h"
#include "rtsputils.h"

#define NMAX 10

int iput[CH_MAX] ; /* ���λ������ĵ�ǰ����λ�� */
int iget[CH_MAX]; /* �������ĵ�ǰȡ��λ�� */
int n [CH_MAX]; /* ���λ������е�Ԫ�������� */

struct ringbuf ringfifo[CH_MAX][NMAX];
extern int UpdateSpsOrPps(unsigned char *data,int frame_type,int len);
extern void UpdateSps(int ch,unsigned char *data,int len);
extern void UpdatePps(int ch,unsigned char *data,int len);
/* ���λ������ĵ�ַ��ż��㺯����������﻽�ѻ�������β�������ƻص�ͷ����
���λ���������Ч��ַ���Ϊ��0��(NMAX-1)
*/
void ringmalloc(int size)
{
    int i,j;
	for(j=0;j<CH_MAX;j++)
	{
		for(i =0; i<NMAX; i++)
		{
			ringfifo[j][i].buffer = malloc(size);
			ringfifo[j][i].size = 0;
			ringfifo[j][i].frame_type = 0;
		}
		
		iput[j] = 0; /* ���λ������ĵ�ǰ����λ�� */
		iget[j] = 0; /* �������ĵ�ǰȡ��λ�� */
		n[j] = 0; /* ���λ������е�Ԫ�������� */

	}

}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ringreset(int ch)
{
	iput[ch]= 0; /* ���λ������ĵ�ǰ����λ�� */
	iget[ch] = 0; /* �������ĵ�ǰȡ��λ�� */
	n[ch] = 0; /* ���λ������е�Ԫ�������� */
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ringfree(void)
{
    int i,j;
    printf("begin free mem\n");
	for(j=0;j<CH_MAX;j++)
	{
		for(i =0; i<NMAX; i++)
		{
			free(ringfifo[j][i].buffer);
			ringfifo[j][i].size = 0;
		}
	}
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int addring(int i)
{
    return (i+1) == NMAX ? 0 : i+1;
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
/* �ӻ��λ�������ȡһ��Ԫ�� */

int ringget(int ch,struct ringbuf *getinfo)
{
    int Pos;
    if(n[ch]>0)
    {
        Pos = iget[ch];
        iget[ch]= addring(iget[ch]);
        n[ch]--;
        getinfo->buffer = (ringfifo[ch][Pos].buffer);
        getinfo->frame_type = ringfifo[ch][Pos].frame_type;
        getinfo->size = ringfifo[ch][Pos].size;
        return ringfifo[ch][Pos].size;
    }
    else
    {
        return 0;
    }
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
/* ���λ������з���һ��Ԫ��*/
void ringput(int ch,unsigned char *buffer,int size,int encode_type)
{

    if(n[ch]<NMAX)
    {
        memcpy(ringfifo[ch][iput[ch]].buffer,buffer,size);
        ringfifo[ch][iput[ch]].size= size;
        ringfifo[ch][iput[ch]].frame_type = encode_type;
        iput[ch] = addring(iput[ch]);
        n[ch]++;
    }
    else
    {
        //  printf("Buffer is full\n");
    }
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
HI_S32 HisiPutH264DataToBuffer(int ch,VENC_STREAM_S *pstStream)
{
	HI_S32 i;
	HI_S32 len=0,off=0;
	unsigned char *pstr;
	int iframe=0;
	for (i = 0; i < pstStream->u32PackCount; i++)
	{
		len+=pstStream->pstPack[i].u32Len[0];

		if (pstStream->pstPack[i].u32Len[1] > 0)
		{
			len+=pstStream->pstPack[i].u32Len[1];
		}
	}


    if(n[ch]<NMAX)
    {
		for (i = 0; i < pstStream->u32PackCount; i++)
		{
			memcpy(ringfifo[ch][iput[ch]].buffer+off,pstStream->pstPack[i].pu8Addr[0],pstStream->pstPack[i].u32Len[0]);
			off+=pstStream->pstPack[i].u32Len[0];
			pstr=pstStream->pstPack[i].pu8Addr[0];

			if(pstr[4]==0x67)
			{
				UpdateSps(ch,ringfifo[ch][iput[ch]].buffer+off,9);
				iframe=1;
			}
			if(pstr[4]==0x68)
			{
				UpdatePps(ch,ringfifo[ch][iput[ch]].buffer+off,4);
			}

			if (pstStream->pstPack[i].u32Len[1] > 0)
			{
				memcpy(ringfifo[ch][iput[ch]].buffer+off,pstStream->pstPack[i].pu8Addr[1], pstStream->pstPack[i].u32Len[1]);
				off+=pstStream->pstPack[i].u32Len[1];
			}
		}

        ringfifo[ch][iput[ch]].size= len;
		if(iframe)
		{
			ringfifo[ch][iput[ch]].frame_type = FRAME_TYPE_I;
		}
        	
		else
			ringfifo[ch][iput[ch]].frame_type = FRAME_TYPE_P;
        iput[ch] = addring(iput[ch]);
        n[ch]++;
    }

	return HI_SUCCESS;
}
