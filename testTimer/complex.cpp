#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

void termination_handler(int signum)
{
    printf("Timer scaduto\n");
    exit(0);
}

int main()
{
    if (signal(SIGUSR1, termination_handler) == SIG_ERR)
        printf("Cannot register SIGUSR1 handler\n");

    printf("Start\n");
    printf("1\n");
    struct sigevent sigeventStruct; // sigevent struct that will be used by timer1 timer
    printf("2\n");
    memset(&sigeventStruct, 0, sizeof sigeventStruct); // zero initialize struct
    printf("3\n");
    sigeventStruct.sigev_notify = SIGEV_SIGNAL; // kind of notification of timer1 expiration
    printf("4\n");
    sigeventStruct.sigev_signo = 10;
    printf("5\n");

    timer_t timer1; // create a timer identifier
    printf("6\n");
    if(timer_create(_POSIX_MONOTONIC_CLOCK, &sigeventStruct, &timer1) == -1)
    {
        printf( "Errore timer_create: %s\n", strerror( errno ) );
    }

    printf("7\n");
    struct itimerspec tempoIniziale;
    printf("8\n");
    memset(&tempoIniziale, 0, sizeof tempoIniziale); // zero initialize struct
    printf("9\n");
    tempoIniziale.it_value.tv_nsec = 100000000;
    //tempoIniziale.it_interval.tv_nsec = 10000;
    printf("10\n");


    if(timer_settime(timer1, 0, &tempoIniziale, NULL) == -1) // timer armed
    {
        printf( "Errore timer_settime: %s\n", strerror( errno ) );
    }
    printf("11\n");
    for(int i = 0; i< 10; i++)
    {
        printf("ciclo %d\n", i);
    }

    struct sigaction oldSigAzione;
    printf("12\n");
    memset(&oldSigAzione, 0, sizeof oldSigAzione);
    printf("13\n");
    oldSigAzione.sa_handler = termination_handler;
    printf("14\n");
    sigemptyset (&oldSigAzione.sa_mask);

    printf("15\n");
    oldSigAzione.sa_flags = 0;

    printf("16\n");
    sigaction (SIGEV_SIGNAL, NULL, &oldSigAzione);
    printf("17\n");
    if(oldSigAzione.sa_handler == SIG_IGN)
    {
        printf("Segnale ignorato\n");
    }
    printf("18\n");
    for(long i = 0; i < 10000000000000000; i++)
    {

    }
    printf("19\n");
    printf("number of expirations %d\n", timer_getoverrun(timer1));
    return 0;
}