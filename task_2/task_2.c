#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef struct {
	int txd[2];
	int rxd[2];
} dpipe_t;

int main() {
	char buf[4096];
	size_t s;
	dpipe_t dp;
	if (pipe(dp.txd) < 0 || pipe(dp.rxd) < 0) {
		printf("pipe failed\n");
		exit(-1);
	}
	pid_t pid = fork();
	if (pid == -1) {
		printf("fork failed\n");
		exit(-1);
	}
	if (pid) {
		close(dp.txd[0]);
		close(dp.rxd[1]);
		while((s = read(0, buf, sizeof(buf) - 1))) {
			buf[s] = 0;
			write(dp.txd[1], buf, s);
			printf("parent sent to child: %s\n", buf);
			s = read(dp.rxd[0], buf, sizeof(buf) - 1);
			buf[s] = 0;
			printf("parent received from child: %s\n", buf);
		}
	}
	if (pid == 0) {
		close(dp.txd[1]);
		close(dp.rxd[0]);
		while((s = read(0, buf, sizeof(buf) - 1))) {
			buf[s] = 0;
			write(dp.rxd[1], buf, s);
			printf("child sent to parent: %s\n", buf);
			s = read(dp.txd[0], buf, sizeof(buf) - 1);
			buf[s] = 0;
			printf("child received from parent: %s\n", buf);
		}
	}
	waitpid(pid, NULL, 0);
	return 0;
}
