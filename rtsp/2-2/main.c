
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>

#include "rtspservice.h"
#include "rtputils.h"
#include "ringfifo.h"
//#include "sample_comm.h"
//#include "hi_mpi_nextchip_process.h"
extern int g_s32Quit ;
//VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_PAL;

extern void PrefsInit();
extern void RTP_port_pool_init(int port);
extern void EventLoop(int s32MainFd);
#if 0
/**************************************************************************************************
**
**HI3531 8路视频监控程序
**
**************************************************************************************************/
void * SAMPLE_VENC_4D1_H264(void *arg)
{
    SAMPLE_VI_MODE_E enViMode = SAMPLE_VI_MODE_8_D1;
    //SAMPLE_VI_MODE_E enViMode = SAMPLE_VI_MODE_16_D1;

    HI_U32 u32ViChnCnt = 8;
    //HI_U32 u32ViChnCnt = 16;
    HI_S32 s32VpssGrpCnt = 8;
    //HI_S32 s32VpssGrpCnt = 16;
    PAYLOAD_TYPE_E enPayLoad[2]= {PT_H264, PT_H264};
    PIC_SIZE_E enSize[2] = {PIC_D1, PIC_CIF};
    
    VB_CONF_S stVbConf;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr;
    VENC_GRP VencGrp;
    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode;
    
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
//    HI_CHAR ch;
    SIZE_S stSize;
   // int ViChn;
    /******************************************
     step  1: init variable 
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,PIC_D1, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.u32MaxPoolCnt = 128;
    
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = u32ViChnCnt * 6;
    memset(stVbConf.astCommPool[0].acMmzName,0,
        sizeof(stVbConf.astCommPool[0].acMmzName));

    /* hist buf*/
    stVbConf.astCommPool[1].u32BlkSize = (196*4);
    stVbConf.astCommPool[1].u32BlkCnt = u32ViChnCnt * 6;
    memset(stVbConf.astCommPool[1].acMmzName,0, sizeof(stVbConf.astCommPool[1].acMmzName));

    /******************************************
     step 2: mpp system init. 
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_8D1_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    s32Ret = SAMPLE_COMM_VI_Start(enViMode, gs_enNorm);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_8D1_0;
    }
    
    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, PIC_D1, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_8D1_0;
    }
    
    stGrpAttr.u32MaxW = stSize.u32Width;
    stGrpAttr.u32MaxH = stSize.u32Height;
    stGrpAttr.bDrEn = HI_FALSE;
    stGrpAttr.bDbEn = HI_FALSE;
    stGrpAttr.bIeEn = HI_TRUE;
    stGrpAttr.bNrEn = HI_TRUE;
    stGrpAttr.bHistEn = HI_TRUE;
    stGrpAttr.enDieMode = VPSS_DIE_MODE_AUTO;
    stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;

    s32Ret = SAMPLE_COMM_VPSS_Start(s32VpssGrpCnt, &stSize, VPSS_MAX_CHN_NUM,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!\n");
        goto END_VENC_8D1_1;
    }


    s32Ret = SAMPLE_COMM_VI_BindVpss(enViMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        goto END_VENC_8D1_2;
    }

    /******************************************
     step 5: select rc mode
    ******************************************/
	 enRcMode = SAMPLE_RC_CBR;

    /******************************************
     step 5: start stream venc (big + little)
    ******************************************/
    for (i=0; i<u32ViChnCnt; i++)
    //for (i=0; i<u32ViChnCnt/2; i++)
    {
        /*** main stream **/
		#if 0
        VencGrp = i*2;
        VencChn = i*2;
		#else
		 VencGrp =i;
        VencChn = i;
		#endif
        VpssGrp = i;
		
		/*if( i == 4 )
		VpssGrp = 8;
		else if( i == 5 )
		VpssGrp = 9;
		else if( i == 6 )
		VpssGrp = 10;
		else if( i == 7 )
		VpssGrp = 11;
		*/
        s32Ret = SAMPLE_COMM_VENC_Start(VencGrp, VencChn, enPayLoad[0],\
                                       gs_enNorm, enSize[0], enRcMode);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_8D1_2;
        }

        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencGrp, VpssGrp, VPSS_BSTR_CHN);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_8D1_3;
        }
		#if 0
        /*** Sub stream **/
        VencGrp ++;
        VencChn ++;
        s32Ret = SAMPLE_COMM_VENC_Start(VencGrp, VencChn, enPayLoad[1], gs_enNorm, enSize[1], enRcMode);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_8D1_3;
        }

        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VPSS_PRE0_CHN);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_8D1_3;
        }
		#endif
    }

    /******************************************
     step 6: stream venc process -- get stream, then save it to file. 
    ******************************************/
    //s32Ret = SAMPLE_COMM_VENC_StartGetStream(u32ViChnCnt/2);
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(u32ViChnCnt);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_8D1_3;
    }

    //printf("please press twice ENTER to exit this sample\n");
     while (!g_s32Quit)
     {
		usleep(1000*200);
	 }
    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();
    
END_VENC_8D1_3:
    //for (i=0; i<u32ViChnCnt/2; i++)
    for (i=0; i<u32ViChnCnt; i++)
    {
        VencGrp = i;
        VencChn = i;
        VpssGrp = i;
		/*if( i == 4 )
		VpssGrp = 8;
		else if( i == 5 )
		VpssGrp = 9;
		else if( i == 6 )
		VpssGrp = 10;
		else if( i == 7 )
		VpssGrp = 11;
		*/
        //VpssChn = (VpssGrp%2)?VPSS_PRE0_CHN:VPSS_BSTR_CHN;
        VpssChn = VPSS_BSTR_CHN;
        SAMPLE_COMM_VENC_UnBindVpss(VencGrp, VpssGrp, VpssChn);
        SAMPLE_COMM_VENC_Stop(VencGrp,VencChn);
    }
    SAMPLE_COMM_VI_UnBindVpss(enViMode);
END_VENC_8D1_2:	//vpss stop
    SAMPLE_COMM_VPSS_Stop(s32VpssGrpCnt, VPSS_MAX_CHN_NUM);
END_VENC_8D1_1:	//vi stop
    SAMPLE_COMM_VI_Stop(enViMode);
END_VENC_8D1_0:	//system exit
    SAMPLE_COMM_SYS_Exit();
	printf("exit enc pthread\n");
    pthread_exit(0);
    return NULL;
}
#endif


/**************************************************************************************************
**
**
**
**************************************************************************************************/

//#include "stream_main.h"
void * get_file_stream(void *arg)
{
//	stream_main();  //mxc_vpu_test视频录像程序代码主程序

	OpenBitstreamFile();
	while(1)
	{
		GetAnnexbNALU();
		usleep(1000*100);
	}
}



/**************************************************************************************************
**
**
**
**************************************************************************************************/
int main(void)
{
	int s32MainFd;
	
	pthread_t id;
	ringmalloc(720*576);
	printf("RTSP server START\n");
	PrefsInit();
	printf("listen port %d for client connecting...\n", SERVER_RTSP_PORT_DEFAULT);
	signal(SIGINT, IntHandl);
	s32MainFd = tcp_listen(SERVER_RTSP_PORT_DEFAULT);
	if (ScheduleInit() == ERR_FATAL)
	{
		fprintf(stderr,"Fatal: Can't start scheduler %s, %i \nServer is aborting.\n", __FILE__, __LINE__);
		return 0;
	}
	
	/* 将所有可用的RTP端口号放入到port_pool[MAX_SESSION] 中 */
    RTP_port_pool_init(RTP_DEFAULT_PORT);
	
	pthread_create(&id,NULL,get_file_stream,NULL);
	while (!g_s32Quit)
	{
		//nanosleep(&ts, NULL);
		EventLoop(s32MainFd);
		usleep(1000*200);
	}
	sleep(2);
	ringfree();
	printf("The Server quit!\n");

	return 0;
}

