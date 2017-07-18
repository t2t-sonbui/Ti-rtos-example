/*
 * timer_thread.h
 *
 *  Created on: Jun 20, 2017
 *      Author: Son Bui
 */
/* POSIX Header files */
#include <pthread.h>

#include <ti/drivers/Timer.h>
#ifndef __TIMER_THREAD_H
#define __TIMER_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif
extern pthread_t timerthread_handler;
extern void *timerThread(void *arg0);
extern void blinkTimer_Callback(Timer_Handle myHandle);

extern Timer_Handle blinkTimer;
extern Timer_Params timer_params;
extern sem_t semEnableTimer;


#endif /* __TIMER_THREAD_H_ */
