#ifndef __QUEUE_H__
#define  __QUEUE_H__

void queue_init(void);
void queue_uninit(void);
DVR_Queue_T * GetVideoQueueBufferAddr(int channel);

#endif

