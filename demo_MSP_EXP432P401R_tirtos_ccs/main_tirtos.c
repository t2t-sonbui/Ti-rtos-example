/*
 *  ======== main_tirtos.c ========
 */
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>

/* Example/Board Header files */
#include "Board.h"

//extern void *mainThread(void *arg0);

/* Local Header Files */
#include "led_thread.h"
#include "gpio_thread.h"
#include "uart_thread.h"
#include "timer_thread.h"

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

/*
 *  ======== main ========
 */
int main(void)
{

    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    /* Call driver init functions */
    Board_initGeneral();

    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if (retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while (1)
            ;
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if (retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while (1)
            ;
    }

    retc = pthread_create(&gpiothread_handler, &pAttrs, gpioThread, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    /* Create Uart Thread with priority = 1 */
    priParam.sched_priority = 1; //Bang muc uu tien cua GPIO thread, neeus khgac phai su dung mutex vi cung dieu khin trang thai led
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&uartthread_handler, &pAttrs, uartThread, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    /* Create LED Thread with priority = 2 */
    priParam.sched_priority = 2;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&ledthread_handler, &pAttrs, ledThread, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }
    /* Create Timer Blink with priority = 3 */
    priParam.sched_priority = 3;
    pthread_attr_setschedparam(&pAttrs, &priParam);
    retc = pthread_create(&timerthread_handler, &pAttrs, timerThread, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    BIOS_start();

    return (0);
}
