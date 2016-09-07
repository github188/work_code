#ifndef __KEY_QUEUE_INC__
#define __KEY_QUEUE_INC__

#define DVR_QUEUE_MAX_SIZE  65535
typedef struct
{
    void        **Queue;
    unsigned int        *count;
	unsigned int        *isValidQueue;
	unsigned int        *ref;
	unsigned int        *IsIframe;
	long long       *current_time_usec;
    unsigned int        QueueTail;
    unsigned int        CurPointer;
    unsigned int        Enable;
    unsigned int        MaxQueue;
    unsigned int        MaxQueutUnitSize;
    pthread_mutex_t     QueueMutex;             //Key mutex
} DVR_Queue_T;

DVR_Queue_T* DVR_CreateNewQueue(unsigned int  dwMaxQueueSize,unsigned int  dw_QueutUnitSize);
     int   DVR_DestroyQueueNode(DVR_Queue_T *KeyQueue,unsigned int  dwMaxQueueSize);
     void    DVR_PutFifoQueueNode(DVR_Queue_T *KeyQueue, void *data,unsigned int  length,long long times,unsigned int  IsIframe);
      int    DVR_GetFifoQueueNode(DVR_Queue_T *KeyQueue,char *buffer,unsigned int  *length,long long *times,unsigned int  *IsIframe);
     unsigned int    DVR_GetFifoQueueSize(DVR_Queue_T *KeyQueue);
     int	 DVR_ClearFifoQueueSize(DVR_Queue_T *KeyQueue);
	 int     DVR_GetFifoQueueNodeTimeStamps(DVR_Queue_T *KeyQueue,long long *times);
#endif
