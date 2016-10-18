/*ringbuf .c*/

#include<stdio.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ringfifo.h"
#include "rtputils.h"
//#include "sample_comm.h"
#include "rtsputils.h"


#define NMAX 32

int test_fist=0;

int iput = 0; /* ���λ������ĵ�ǰ����λ�� */

int iget = 0; /* �������ĵ�ǰȡ��λ�� */

int n = 0; /* ���λ������е�Ԫ�������� */

struct ringbuf ringfifo[NMAX];

int HisiPutH264DataToBuffer(void *buf,int size);

extern int UpdateSpsOrPps(unsigned char *data,int frame_type,int len);
extern void UpdateSps(int ch,unsigned char *data,int len);
extern void UpdatePps(int ch,unsigned char *data,int len);

//*****************************add by zhu***************************

typedef struct
{
  int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
  unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
  unsigned max_size;            //! Nal Unit Buffer size
  int forbidden_bit;            //! should be always FALSE
  int nal_reference_idc;        //! NALU_PRIORITY_xxxx
  int nal_unit_type;            //! NALU_TYPE_xxxx    
  char *buf;                    //! contains the first byte followed by the EBSP
  unsigned short lost_packets;  //! true, if packet loss is detected
} NALU_t;

//ΪNALU_t�ṹ������ڴ�ռ�
NALU_t *AllocNALU(int buffersize)
{
  NALU_t *n;

  if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
  {
	  printf("AllocNALU: n");
	  exit(0);
  }

  n->max_size=buffersize;

  if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)
  {
    free (n);
    printf ("AllocNALU: n->buf");
	exit(0);
  }

  return n;
}

//��Ӷ�ȡ�ļ��е�h264֡
FILE *fd;
static int info3=0, info4=0;

void OpenBitstreamFile()
{
	if( NULL == (fd = fopen("test.264", "rb")))
	{
		printf("open test.264 file error \n");
		exit(0);
	}
}

static int FindStartCode3 (unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; //�ж��Ƿ�Ϊ0x000001,����Ƿ���1
	else return 1;
}

static int FindStartCode4 (unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;//�ж��Ƿ�Ϊ0x00000001,����Ƿ���1
	else return 1;
}


int GetAnnexbNALU()
{
	int pos = 0;
	int StartCodeFound, rewind;
//	unsigned char Buf[30000];
	unsigned char *Buf;
	Buf = malloc(40000);
	memset(Buf, 0 ,40000);
	int startcodeprefix_len =3; //��ʼ���������еĿ�ʼ�ַ�Ϊ3���ֽ� 00 00 01

	if( 3 != fread(Buf, 1, 3, fd))//�������ж�3���ֽ�
	{
		printf("read error \n");
		free(Buf);
		return 0;
	}
	info3 = FindStartCode3(Buf);//�ж��Ƿ�Ϊ0x000001 
	if(info3 != 1)
	{
		//������ǣ��ٶ�һ���ֽ�
		if( 1 != fread(Buf+3, 1, 1, fd))
		{
			printf("read error \n");
			free(Buf);
			return 0;
		}
		info4 = FindStartCode4(Buf);//�ж��Ƿ�Ϊ0x00000001
		if( info4 != 1)
		{
			free(Buf);
			return 0;
		}
		else
		{
			//�����0x00000001,�õ���ʼǰ׺Ϊ4���ֽ�
			pos = 4;
			startcodeprefix_len = 4;
		}
	}
	else
	{
		//�����0x000001,�õ���ʼǰ׺Ϊ3���ֽ�
		startcodeprefix_len =3;
		pos = 3;
	}
	
	StartCodeFound = 0;
	info3 = 0;
	info4 = 0;

	while( !StartCodeFound )
	{
		if(feof(fd))//�ж��Ƿ����ļ�β
		{
			goto loop;
		}
		Buf[pos++] = fgetc(fd);//��һ���ֽڵ�BUF��
		info4 = FindStartCode4(&Buf[pos-4]);
		if(info4 != 1)
			info3 = FindStartCode3(&Buf[pos-3]);
		StartCodeFound = (info3 == 1 || info4 == 1);
	}

	rewind = (info4 == 1)? -4 : -3;
	if( 0 != fseek(fd, rewind, SEEK_CUR))//���ļ�ָ��ָ��ǰһ��NALU��ĩβ
	{
		free(Buf);
		printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
	}
	
	HisiPutH264DataToBuffer(Buf, pos+rewind);
	free(Buf);
	return (pos+rewind);
loop:
	HisiPutH264DataToBuffer(Buf, pos-1);
	fseek(fd,0,SEEK_SET); //����ȡ���ļ�������ʱ����´�ͷ��ȡ�ļ����ݣ�
	free(Buf);
	return pos-1;

}

//******************************************************************

/* ���λ������ĵ�ַ��ż��㺯����������﻽�ѻ�������β�������ƻص�ͷ����

   ���λ���������Ч��ַ���Ϊ��0��(NMAX-1)

*/
void ringmalloc(int size)
{
	int i;
	for (i =0;i<NMAX;i++) 
	{
		ringfifo[i].buffer = malloc (size);
		ringfifo[i].size = 0;
		ringfifo[i].frame_type = 0;
		printf("FIFO INFO:idx:%d,len:%d,ptr:%x\n",i,ringfifo[i].size,(int)(ringfifo[i].buffer));
	}
	iput = 0; /* ���λ������ĵ�ǰ����λ�� */

	iget = 0; /* �������ĵ�ǰȡ��λ�� */

	n = 0; /* ���λ������е�Ԫ�������� */
}
void ringreset()
{
	iput = 0; /* ���λ������ĵ�ǰ����λ�� */

	iget = 0; /* �������ĵ�ǰȡ��λ�� */

	n = 0; /* ���λ������е�Ԫ�������� */
}
void ringfree()
{
	int i;
	printf("begin free mem\n");
	for (i =0;i<NMAX;i++)
	{
		printf("FREE FIFO INFO:idx:%d,len:%d,ptr:%x\n",i,ringfifo[i].size,(int)(ringfifo[i].buffer));
		free(ringfifo[i].buffer);
		ringfifo[i].size = 0;
	}
}
int addring (int i)
{
	return (i+1) == NMAX ? 0 : i+1;
}

/* �ӻ��λ�������ȡһ��Ԫ�� */

int ringget(struct ringbuf *getinfo)
{
	int Pos;
	if (n>0){
		Pos = iget;
		iget = addring(iget);
		n--;
		getinfo->buffer = (int)(ringfifo[Pos].buffer);
		getinfo->frame_type = ringfifo[Pos].frame_type;
		getinfo->size = ringfifo[Pos].size;
		//printf("Get FIFO INFO:idx:%d,len:%d,ptr:%x,type:%d\n",Pos,getinfo->size,(int)(getinfo->buffer),getinfo->frame_type);
		return ringfifo[Pos].size;
	}
	else {
		//printf("Buffer is empty\n");
		return 0;
	}
}



/* ���λ������з���һ��Ԫ��*/

void ringput(unsigned char *buffer,int size,int encode_type)
{
	int dest;
	if (n<NMAX){
		dest =memcpy(ringfifo[iput].buffer,buffer,size);
		ringfifo[iput].size= size;
		ringfifo[iput].frame_type = encode_type;
		//printf("Put FIFO INFO:idx:%d,len:%d,ptr:%x,type:%d\n",iput,ringfifo[iput].size,(int)(ringfifo[iput].buffer),ringfifo[iput].frame_type);
		iput = addring(iput);
		n++;
	}
	else 
	{
		//	printf("Buffer is full\n");
	}
}
#if 1
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int HisiPutH264DataToBuffer(void *buf,int size)
{
   // HI_S32 i;
	unsigned char *pstr;
    int iframe=0;
	int off=0;
int i;
	pstr = buf;
    if(n<NMAX)
    {   
        {   
            memcpy(ringfifo[iput].buffer+off,pstr,size);
	
			for(i=0;i<32;i++){
				printf("[%x]",pstr[i]);
			}
			printf("iput = %d \n",iput);
          
            if(pstr[4]==0x67)
            {   
                UpdateSps(0,ringfifo[iput].buffer+4,9);
                iframe=1;
            }   
            if(pstr[4]==0x68)
            {   
                UpdatePps(0,ringfifo[iput].buffer+4,5);
            }  
        }	
printf("1\n");
        ringfifo[iput].size= size;
        if(iframe)
        {   
            ringfifo[iput].frame_type = FRAME_TYPE_I;
        }

        else
            ringfifo[iput].frame_type = FRAME_TYPE_P;
        iput = addring(iput);
        n++;
    }
printf("2\n");

    return 1;
}
#endif

