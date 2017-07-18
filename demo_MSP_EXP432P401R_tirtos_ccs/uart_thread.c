/*
 *  ======== uart_thread.c ========
 */

/* Includes */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* POSIX Header Files */
#include <semaphore.h>

/* Driver Header Files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Board Header Files */
#include "Board.h"

/* Local Header Files */
#include "gpio_thread.h"
#include "led_thread.h"

/* Global Variables */
volatile int bytesReceived = 1;     // Keeps track of # UART bytes received

pthread_t uartthread_handler;
sem_t semRepUpdate;
UART_Handle uart;
const char ledOnState[] = "\fled On\r\n";
const char ledOffState[] = "\fled Off\r\n";
char receiveByte;
/* Stack size in bytes */
#define THREADSTACKSIZE    1024
/*  ======== RepUpdateFxn ========
 *  Response state of led to uart.
 */
void *RepUpdateFxn(void *arg0)
{
    int rc;
    /* Initialize Semaphore */
    rc = sem_init(&semRepUpdate, 0, 0);
    if (rc == -1)
    {
        while (1)
            ;
    }
    while (1)
    {
        /* Timer tells when to toggle LED On/Off*/
        rc = sem_wait(&semRepUpdate);
        if (rc == 0)
        {
            if (ledState)
            {
                UART_write(uart, ledOnState, sizeof(ledOnState));
            }
            else
            {
                UART_write(uart, ledOffState, sizeof(ledOffState));
            }
        }
    }
}
/*  ======== RepUpdateFxn ========
 *  Setstate of led to uart.
 */
void *SetUpdateFxn(void *arg0)
{
    int rc;
    while (1)
    {
        UART_read(uart, &receiveByte, 1);  //input = RXD byte
        switch (receiveByte)
        {
        case 'b':
            //Lenh bat den
            ledState = 1; //chu y o day neu ko cung muc do uu tien ngat cua nut bam va den thi can phai dung mutex
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
            break;
        case 't':
            //Lenh bat den
            ledState = 0; //chu y o day neu ko cung muc do uu tien ngat cua nut bam va den thi can phai dung mutex
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
            break;
        default:
            UART_write(uart, &receiveByte, 1);
            break;
        }

    }
}

/*
 *  ======== mainThread ========
 */
void *uartThread(void *arg0)
{

    pthread_t RepUpdateFxn_Handle, SetUpdateFxn_Handle;
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int detachState;
    int rc;
    UART_Params uartParams;

    /* Call driver init functions */
    GPIO_init();
    UART_init();

    /* Define UART parameters*/
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL)
    {
        /* UART_open() failed */
        while (1)
            ;

    }

    /* Create application threads */
    pthread_attr_init(&pAttrs);

    detachState = PTHREAD_CREATE_DETACHED;
    /* Set priority and stack size attributes */
    priParam.sched_priority = 1; //Cung voi muc do uu tien cua thread chinhs
    pthread_attr_setschedparam(&pAttrs, &priParam);

    rc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if (rc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while (1)
            ;
    }

    rc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if (rc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while (1)
            ;
    }

    /* Create RepUpdateFxn thread */
    rc = pthread_create(&RepUpdateFxn_Handle, &pAttrs, RepUpdateFxn, NULL);
    if (rc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    /* Create SetUpdateFxn thread */
    rc = pthread_create(&SetUpdateFxn_Handle, &pAttrs, SetUpdateFxn, NULL);
    if (rc != 0)
    {
        /* pthread_create() failed */
        while (1)
            ;
    }

    return (NULL);


}
