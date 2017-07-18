/*
 *  ======== gpio_thread.c ========
 */
/* Includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

/* POSIX Header Files */
#include <semaphore.h>

/* Driver Header Files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

/* Board Header Files */
#include "Board.h"

/* Local Header Files */

#include "led_thread.h"
#include "uart_thread.h"
#include "timer_thread.h"

/* Global Variables */
pthread_t gpiothread_handler;

/*
 *  ======== gpioButtonFxn0 ========

 */
void gpioButtonFxn0(uint_least8_t index)
{
    //GPIO_disableInt (index);
    int rc;
    ledState = !ledState;
    /* Post to semaphore that rate change occurred. */
    rc = sem_post(&semBlink);
    if (rc == -1)
    {
        while (1)
            ;
    }
    rc = sem_post(&semRepUpdate);
    if (rc == -1)
    {
        while (1)
            ;
    }

}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON1.

 */
void gpioButtonFxn1(uint_least8_t index)
{


    int rc;

    /* Post to enable timer. */
    rc = sem_post(&semEnableTimer);
    if (rc == -1)
    {
        while (1)
            ;
    }
}

/*
 *  ======== mainThread ========
 */
void *gpioThread(void *arg0) //Higher Thread
{

    /* Call driver init functions */
    GPIO_init();

    /* install Button callback */
    GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(Board_GPIO_BUTTON0);

    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on Board_GPIO_BUTTON1.
     */
    if (Board_GPIO_BUTTON0 != Board_GPIO_BUTTON1)
    {
        /* Install Button callback */
        GPIO_setCallback(Board_GPIO_BUTTON1, gpioButtonFxn1);
        GPIO_enableInt(Board_GPIO_BUTTON1);
    }

    return (NULL);
}
