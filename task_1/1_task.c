#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
void run(char *str) {
        const long max_arg = sysconf(_SC_ARG_MAX);
        const pid_t pid = fork();
        if (pid < 0) {
                printf("fork() error\n");
                return;
        }
        if (pid) {
                int status;
                waitpid(pid, &status, 0);
                printf("Ret code: %d\n", WEXITSTATUS(status));
                return;
        }
        char *p;
        int i = 0;
        char delim[] = " \n";
        char **args;
        args = malloc(max_arg * sizeof(char));
        for (p = strtok(str, delim); p != NULL; p = strtok(NULL, delim)) {
                *(args + i) = p;
                i++;
        }
        if (execvp(args[0], args) == -1) {
                printf("exec() error\n");
                free(args);
                exit(1);
        }
        free(args);
        exit(0);
}

int main() {
        const long max_arg = sysconf(_SC_ARG_MAX);
        char *str;
        while(26) {
                str = (char *)malloc(max_arg * sizeof(char));
                fgets(str, sizeof(str), stdin);
                run(str);
                free(str);
        }
        return 0;
}
