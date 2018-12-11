#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

int out = 0, count = 128;
pid_t pid;
void one(int signo) {
        out += count;
        count /= 2;
        kill(pid, SIGUSR1);
}
void zero(int signo) {
        count /=2;
        kill(pid, SIGUSR1);
}
void childexit(int signo) {
	exit(0);
}
void parentexit(int signo) {
	exit(0);
}
void empty(int signo) {
}
int main(int argc, char ** argv){
	int i;
	unsigned int fd = 0;
        char c = 0;
	if (argc != 2)
		return 0;
	pid_t ppid = getpid();
	sigset_t set;
	struct sigaction act_exit;
	memset(&act_exit, 0, sizeof(act_exit));
	act_exit.sa_handler = childexit;
	sigfillset(&act_exit.sa_mask);
	sigaction(SIGCHLD, &act_exit, NULL);
	struct sigaction act_one;
	memset(&act_one, 0, sizeof(act_one));
	act_one.sa_handler = one;
	sigfillset(&act_one.sa_mask);
	sigaction(SIGUSR1, &act_one, NULL);
	struct sigaction act_zero;
	memset(&act_zero, 0, sizeof(act_zero));
	act_zero.sa_handler = zero;
	sigfillset(&act_zero.sa_mask);
	sigaction(SIGUSR2, &act_zero, NULL);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, NULL);
	sigemptyset(&set);
	pid = fork();
	if (pid == 0) {
    		sigemptyset(&set);
		struct sigaction act_empty;
		memset(&act_empty, 0, sizeof(act_empty));
		act_empty.sa_handler = empty;
		sigfillset(&act_empty.sa_mask);
		sigaction(SIGUSR1, &act_empty, NULL);
		struct sigaction act_alarm;
		memset(&act_alarm, 0, sizeof(act_alarm));
		act_alarm.sa_handler = parentexit;
		sigfillset(&act_alarm.sa_mask);
		sigaction(SIGALRM, &act_alarm, NULL);
		if ((fd = open(argv[1], O_RDONLY)) < 0 ){
			perror("Can't open file");
			return -1;
		}
		while (read(fd, &c, 1)){
			alarm(1);
			for (i = 128; i >= 1; i /= 2){
				if ( i & c )
					kill(ppid, SIGUSR1);
				else
					kill(ppid, SIGUSR2);
				sigsuspend(&set);
			}
		}
		return 0;
	}
	errno = 0;
	while(726){
		if(count == 0){
			write(STDOUT_FILENO, &out, 1);
			fflush(stdout);
			count = 128;
      			out = 0;
		}
   		sigsuspend(&set);
	}
	return 0;
}
