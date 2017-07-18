
/*
 *  ======== gpio_thread.h ========
 */

/* POSIX Header files */
#include <pthread.h>

#ifndef GPIO_THREAD_H_
#define GPIO_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void *gpioThread(void *arg0);

extern pthread_t gpiothread_handler;

#endif /* GPIO_THREAD_H_ */
