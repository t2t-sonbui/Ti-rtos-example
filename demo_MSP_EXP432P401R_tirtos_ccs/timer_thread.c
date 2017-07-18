/*
 * timer_thread.c
 *
 *  Created on: Jun 20, 2017
 *      Author: Son Bui
 */
/* Includes */
#include <stddef.h>
#include <time.h>
#include <stdlib.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

/* POSIX Header Files */
#include <semaphore.h>

/* Driver Header Files */
#include <ti/drivers/Timer.h>

/* Board Header Files */
#include "Board.h"

/* Local Header Files */
#include "gpio_thread.h"
#include "led_thread.h"

/* Global Variables */
pthread_t timerthread_handler;
Timer_Handle blinkTimer;
Timer_Params timer_params;
bool enableTimer = 0;
sem_t semEnableTimer;
/* Stack size in bytes */
#define THREADSTACKSIZE    1024

void *EnableTimerFxn(void *arg0)
{
    int rc;

    /* Initialize Semaphore */
    rc = sem_init(&semEnableTimer, 0, 0);
    if (rc == -1)
    {
        while (1)
            ;
    }
    while (1)
    {
        rc = sem_wait(&semEnableTimer);
        if (rc == 0)
        {
            enableTimer = !enableTimer;
            if (enableTimer)
            {
                Timer_start(blinkTimer);
            }
            else
            {
                Timer_stop(blinkTimer);
            }
        }
    }
}

/*  ======== blinkTimer_Callback ========
 * blinkTimer Callback Function
 * This posts a semaphore to ledThread telling the LEDs to toggle ON/OFF.
 */
void blinkTimer_Callback(Timer_Handle myHandle)
{
    int rc;
    /* Post to the TimerBlink task */
    rc = sem_post(&semTimerBlink);
    if (rc == -1)
    {
        while (1)
            ;
    }
}

/*
 *  ======== timerThread ========
 *  Creates timer modules with callback functions.
 */
void *timerThread(void *arg0)
{
    pthread_t enableTimerFxn_Handle;
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    //Seed Random number generator
    srand(TLV->RANDOM_NUM_1);

    /* Call driver init functions */
    Timer_init();

    /* Set-Up blinkTimer */
    Timer_Params_init(&timer_params);
    timer_params.period = 500000;
    timer_params.periodUnits = TIMER_PERIOD_US;
    timer_params.timerMode = TIMER_CONTINUOUS_CB;
    timer_params.timerCallback = blinkTimer_Callback;
    blinkTimer = Timer_open(Board_Timer0, &timer_params);
    if (blinkTimer == NULL)
    {
        /* Failed to initialized Timer */
        while (1)
            ;
    }
    /* Create application threads */
    pthread_attr_init(&pAttrs);

    detachState = PTHREAD_CREATE_DETACHED;
    /* Set priority and stack size attributes */
    priParam.sched_priority = 3;
    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if (retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while (1)
            ;
    }

    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if (retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while (1)
            ;
    }

    /* Create enbaleTimerFxn thread */
    retc = pthread_create(&enableTimerFxn_Handle, &pAttrs, EnableTimerFxn,
                          NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    return (NULL);
}

