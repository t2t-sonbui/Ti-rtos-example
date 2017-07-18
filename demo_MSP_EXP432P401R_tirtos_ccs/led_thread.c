/*
 * led_thread.c
 *
 *  Created on: Jun 19, 2017
 *      Author: Son Bui
 */
/* Includes */
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <stdlib.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>
/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/Timer.h>

/* Board Header files */
#include "Board.h"

/* Local Header Files */

/* Global Variables */
pthread_t ledthread_handler;
bool ledState = 0;
bool ledTimerState = 0;
sem_t semBlink, semTimerBlink;
/* Stack size in bytes */
#define THREADSTACKSIZE    1024
/*  ======== BlinkFxn ========
 *  Blink the LED.
 */
void *BlinkFxn(void *arg0)
{
    int rc;

    /* Initialize Semaphore */
    rc = sem_init(&semBlink, 0, 0);
    if (rc == -1)
    {
        while (1)
            ;
    }
    while (1)
    {
        /* Timer tells when to toggle LED On/Off*/
        rc = sem_wait(&semBlink);
        if (rc == 0)
        {
            if (ledState) // Variable toggled by timer tells if LEDs should be ON/OFF
            {
                GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
            }
            else          // Turn LEDs off by changing duty rate to 0%
            {
                GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
            }
        }
    }
}

/*  ======== BlinkFxn ========
 *  Blink the LED.
 */
void *TimerBlinkFxn(void *arg0)
{
    int rc;

    /* Initialize Semaphore */
    rc = sem_init(&semTimerBlink, 0, 0);
    if (rc == -1)
    {
        while (1)
            ;
    }
    while (1)
    {
        /* Timer tells when to toggle LED On/Off*/
        rc = sem_wait(&semTimerBlink);
        if (rc == 0)
        {
            ledTimerState=!ledTimerState;
            if (ledTimerState) // Variable toggled by timer tells if LEDs should be ON/OFF
            {
                GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_ON);
            }
            else          // Turn LEDs off by changing duty rate to 0%
            {
                GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
            }
        }
    }
}

/*
 *  ======== ledThread ========
 *  Creates the PWM modules for RGB color setting.
 *  Creates individual threads for changing the blink
 *   rate, color and ON/OFF state of the LED.
 */
void *ledThread(void *arg0)
{
    pthread_t BlinkFxn_Handle, TimerBlinkFxn_Handle;
    pthread_attr_t pAttrs;
    struct sched_param priParam;

    int retc;
    int detachState;

    /* Call driver init functions. */
    GPIO_init();

    /* Create application threads */
    pthread_attr_init(&pAttrs);

    detachState = PTHREAD_CREATE_DETACHED;
    /* Set priority and stack size attributes */
    priParam.sched_priority = 1;
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

    /* Create BlinkFxn thread */
    retc = pthread_create(&BlinkFxn_Handle, &pAttrs, BlinkFxn, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }
    /* Create TimerBlinkFxn thread */
    retc = pthread_create(&TimerBlinkFxn_Handle, &pAttrs, TimerBlinkFxn, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    return (NULL);
}
