#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <resolv.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

void dff(const char *file, const char *name);
int tp(const char *name);
void sdir(const char *name);
int main(){
	char direct[256]="/home/domashenko/3_sem/task_6";
	sdir(direct);
	return 0;
}
void sdir(const char *name) {
        DIR *dir = opendir(name);
        if(dir) {
                char path[PATH_MAX], *endp = path;
                strcpy(path, name);
                struct stat inf;
                struct dirent *a;
		endp += strlen(name);
                while ((a = readdir(dir)) != NULL) {
                	char add[200]="/";
                        strcat(add,a->d_name);
                        strcpy(endp, add);
                        if (!stat(path, &inf))
                                if (S_ISDIR(inf.st_mode)) {
                                        if(a->d_name[0]!='.')
                                                sdir(path);
                                }
                                else
                                        if (S_ISREG(inf.st_mode))
                                                if(tp(a->d_name) == 1)
                                                        dff(path,a->d_name);
                }
        }
        return;
}
void dff(const char *file, const char *name){
        char newfile[256] = "/home/domashenko/backup/", log[256] = "/home/domashenko/backup/log.txt", arg[256] = "diff -q ", file2[256] = "", buf[1024];
        strcat(newfile, name);
        strcat(file2, file);
        strcat(file2, " ");
        strcat(file2, newfile);
        strcat(arg, file2);
        int fdn = open(newfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH), fd = open(log, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH);
        if(fd < 0){
		printf("open failed\n");
		exit(EXIT_FAILURE);
	}
        dup2(fdn, 1);
        system(arg);
        lseek(fd, 0, SEEK_SET);
        int x = read(fd, buf, 1024);
        if (x != 0) {
                char copy[256] = "cp ";
                strcat(copy, file2);
                system(copy);
        }
        system("rm /home/domashenko/backup/log.txt");
        close(fd);
        close(fdn);
        return;
}
int tp(const char *name){
        int n = strlen(name);
        if(name[n-1] =='t' && name[n-2] =='x' && name[n-3] =='t'&& name[n-4] =='.'){
                return 1;
        }
        if(name[n-1] == 'c' && name[n-2] =='.'){
                return 1;
        }
        return 0;
}
