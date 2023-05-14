#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
int main(){
	struct sched_param shdprm; // Значения параметров планирования
	int i, pid,ppid;
	pid = getpid();
	ppid = getppid();
	printf("SON_1 PARAMS: pid = %d ppid = %d\n",pid,ppid);
	printf ("SON_1: Текущая политика планирования для текущего процесса: ");
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

	if (sched_getparam (0, &shdprm) == 0)
		printf ("SON_1: Текущий приоритет текущего процесса: %d\n", shdprm.sched_priority);
	else
		perror ("SCHED_GETPARAM");
	return 0;
}
