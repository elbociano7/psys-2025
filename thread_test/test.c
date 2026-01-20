#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/_pthread/_pthread_t.h>

void* thr_job(void* arg) {
	printf("thread job\n");
	return NULL;
}

int main() {	
	pthread_t thread1;

	pthread_create(&thread1, NULL, thr_job, NULL);


	pthread_join(thread1, NULL);

	return 0;	
}
