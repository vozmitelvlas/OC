#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sched.h>
int main (void) {
	struct sched_param shdprm; // Значения параметров планирования
	struct timespec tmin;
	int pid, pid1, pid2, ppid, status;
	pid = getpid();

	sched_rr_get_interval(0, &tmin);
	printf("time left %ld\n", tmin.tv_sec + tmin.tv_nsec/1000000000);

	ppid = getppid();
	printf("FATHER PARAM: pid = %d ppid = %d\n", pid, ppid);
	shdprm.sched_priority = 50;
	if (sched_setscheduler (0, SCHED_RR, &shdprm) == -1) {
		perror ("SCHED_SETSCHEDULER");
	}

	pid1 = fork();
	if(pid1 == 0){
		shdprm.sched_priority = 50;
		sched_setscheduler(50, SCHED_RR, &shdprm);
		execl("son", "son", NULL);
	}
	printf ("Текущая политика планирования для текущего процесса: ");

	switch (sched_getscheduler (0)){
                case SCHED_FIFO:
                        printf ("SCHED__FIFO\n"); break;
                case SCHED_RR:
                        printf ("SCHED_RR\n"); break;
                case SCHED_OTHER:
                        printf ("SCHED_OTHER\n"); break;
                case -1:
                        perror ("SCHED_GETSCHEDULER"); break;
                default:
                        printf ("Unknown policy\n");
        }

	if (sched_getparam(0, &shdprm) == 0)
		printf ("Текущий приоритет текущего процесса: %d\n", shdprm.sched_priority);
	else
		perror ("SCHED_GETPARAM");

	wait(&status);
	return 0;
}

