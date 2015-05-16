#include <iostream>
#include <cstdint>
#include <signal.h>
#include <time.h>
#include <unistd.h>

using namespace std;

timer_t t1;
timer_t t2;

void sigalrm_handler(int sig, siginfo_t *si, void *uc ) {
    timer_t *tidp;
    tidp = (timer_t*)si->si_value.sival_ptr;
    if (*tidp == t1) {
		cout << "Timer 1 triggered" << endl;
    } else if (*tidp == t2) {
		cout << "Timer 2 triggered" << endl;
    }
}

void sigint_handler(int signo) {
	if (signo == SIGINT) {
		cout << "\nReceived SIGINT" << endl;
		exit(0);
	}
}

static int makeTimer( timer_t *timerID, int expireMS, int intervalMS ) {
	struct sigevent te;
	struct itimerspec its;
	struct sigaction sa;
	int sigNo = SIGRTMIN;

	/* Set up signal handler. */
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = sigalrm_handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(sigNo, &sa, NULL) == -1) {
		perror("sigaction");
	}

	/* Set and enable alarm */
	te.sigev_notify = SIGEV_SIGNAL;
	te.sigev_signo = sigNo;
	te.sigev_value.sival_ptr = timerID;
	timer_create(CLOCK_REALTIME, &te, timerID);

	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = intervalMS * 1000000;
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = expireMS * 1000000;
	timer_settime(*timerID, 0, &its, NULL);

	return 1;
}

int main() {
	if (signal(SIGINT, &sigint_handler) == SIG_ERR) {
		cout << "Cannot register SIGINT handler" << endl;
		exit(1);
	}
	makeTimer(&t1, 100, 100);
	makeTimer(&t2, 50, 50);
	// struct itimerval timer = {0};
	// /* Initial timeout value */
	// timer.it_value.tv_sec = 0;
	// timer.it_value.tv_usec = 500000;

	// /* We want a repetitive timer */
	// timer.it_interval.tv_sec = 0;
	// timer.it_interval.tv_usec = 500000;

	// /* Register Signal handler
	// * And register for periodic timer with Kernel*/
	// if (signal(SIGALRM, &sigalrm_handler) == SIG_ERR) {
	// 	cout << "Cannot register SIGALRM handler" << endl;
	// 	exit(1);
	// }
	// setitimer(ITIMER_REAL, &timer, NULL);
	for (;;) {
		usleep(1000000);
	}
}