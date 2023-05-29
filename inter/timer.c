#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
void timer_handler(int signum) {
    printf("Timer expired\n");
}

int main() {
    struct sigaction sa;
    struct itimerval timer;

    // Set up the signal handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    // Configure the timer to expire after 1 sec
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // Start the timer
    setitimer(ITIMER_REAL, &timer, NULL);

    // Loop forever, waiting for signals
    while(1) {
        sleep(1);
    }

    return 0;
}
