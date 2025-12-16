#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

void close_pipes(int unused[2], int used[2], int close_end) {
	close(unused[0]);
	close(unused[1]);
	close(used[close_end]);
}

int main(int argc, char ** argv) {

	if(argc < 2) {
		printf("Usage: %s number_count", argv[1]);
		exit(1);
	}	

	pid_t pid_1, pid_2;
	int fd_filter[2], fd_sum[2];

	if(pipe(fd_filter) == -1 || pipe(fd_sum) == -1) {
		perror("Pipe error");
		exit(2);
	}

	pid_1 = fork();

	if(pid_1 < 0) {
		perror("Fork 1 error");
		exit(3);
	}

	// Child 1 - FILTER
	if(pid_1 == 0) {
		close(fd_filter[1]);
		close(fd_sum[0]);

		int num;

		while(read(fd_filter[0], &num, sizeof(int)) > 0) {
			printf("[FILTER] Received: %d\n", num);

			if(num % 2 == 0) {
				printf("[FILTER] %d is even, passing\n", num);
				write(fd_sum[1], &num, sizeof(int));
			}
		}
		printf("[FILTER] Closing the pipes...\n");
		close(fd_filter[0]);
		close(fd_sum[1]);
		exit(0);
	}

	pid_2 = fork();

	if(pid_2 < 0) {
		perror("Fork 2 error");
		exit(3);
	}

	// Child 2 - SUM
	if(pid_2 == 0) {
		close_pipes(fd_filter, fd_sum, 1);
		
		int num; 
		int sum = 0;

		while(read(fd_sum[0], &num, sizeof(int)) > 0) {
			printf("[SUM] Received number: %d\n", num);
			sum += num;
			printf("[SUM] Sum is %d\n", sum);
		}
		printf("[SUM] Final sum: %d.\n", sum);
		close(fd_sum[0]);
		exit(0);
	}

	if( pid_1 > 0 && pid_2 > 0) {
		close_pipes(fd_sum, fd_filter, 0);

		// init random with time(NULL) seed
		srand(time(NULL));

		int count = atoi(argv[1]);
		int rand_num = 0;

		for(int i = 0; i < count; i++) {
			rand_num = rand() % 100 + 1;
			printf("[PARENT] Writing random number: %d\n", rand_num);
			write(fd_filter[1], &rand_num, sizeof(int));
		}	

		close(fd_filter[1]);

		printf("[PARENT] Pipe closed.\n");


		wait(NULL);
		wait(NULL);

		printf("Child finished. Stopping parent\n");
	}

	exit(0);
}
