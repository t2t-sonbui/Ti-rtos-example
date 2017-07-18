
/*
 *  ======== led_thread.h ========
 */

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>



#ifndef __LEDTHREAD_H
#define __LEDTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t ledthread_handler;
extern void *ledThread(void *arg0);


extern bool   ledState;

extern sem_t semBlink,semTimerBlink;

#ifdef __cplusplus
}
#endif

#endif /* __LEDTHREAD_H */
