#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

double *result;
long long points;
int threads, threads_count = 0;;
sem_t *sem_result, *sem_thread;

void *count(void *args);
int main(int argc, char *argv[]){
	int error, i;
	double integ = 0;
	if(argc < 2)
		return 0;
	threads = atoll(argv[1]);
	points = 10000000;
	points /= threads;
	if((sem_result = sem_open("/named posix semaphore", O_CREAT, 0776, 0)) == SEM_FAILED || (sem_thread = sem_open("/named posix semaphore", O_CREAT, 0776, 0)) == SEM_FAILED){
		perror("sem_open() failed");
		return -1;
	}
	result = (double *)malloc(threads * sizeof(double));
	pthread_t *id = (pthread_t *)malloc(threads * sizeof(pthread_t));
	srand(time(0));
	struct timespec start, stop;
	if(clock_gettime(CLOCK_MONOTONIC, &start) < 0){
		perror("clock gettime() failed");
		return -1;
	}
	for(i = 0; i < threads; ++i) {
		error = pthread_create(id + i, NULL, &count, NULL);
		if(error)
			printf("creating thread error :[%s]", strerror(error));
		sem_wait(sem_thread);
	}
	for(i = 0; i < threads; ++i)
		sem_wait(sem_result);
	if(clock_gettime(CLOCK_MONOTONIC, &stop) == -1 ) {
		perror("clock gettime() failed");
		return -1;
	}
	double accum = (stop.tv_sec - start.tv_sec) + ( stop.tv_nsec - start.tv_nsec) / 1.0e9;
	printf("%lf\n", accum);
	FILE *fdt = fopen("time.txt", "a");
	fprintf(fdt, "%d: %lf\n", threads, accum);
	fclose(fdt);
	for(i = 0; i < threads; ++i)
		integ += result[i];
	integ /= threads;
	printf("result: %lf\n", integ);
	sem_close(sem_result);
	sem_close(sem_thread);
	free(id);
	free(result);
	return 0;
}
void *count(void *args) {
        int thread_id = threads_count++;
        long long i = 0;
        long long int sum = 0;
        double x, y;
        struct drand48_data rand;
        srand48_r(time(NULL), &rand);
        sem_post(sem_thread);
        for (i = 0; i < points ; ++i) {
                drand48_r(&rand, &x);
                drand48_r(&rand, &y);
                if(y < x * x)
                        ++sum;
        }
        result[thread_id] = (double)sum / points;
        sem_post(sem_result);
        return NULL;
}
