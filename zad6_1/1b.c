#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(-1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(-1);
    }

    if (pid > 0) {
        close(fd[0]);
        long count = 0;
        char c = 'A';
        
        printf("[P] writing \n");

        while(1) {
            if (write(fd[1], &c, 1) != 1) {
                perror("[P] Broken pipe");
                break;
            }
            count++;
            if (count % 8192 == 0) {
                printf("[P] Zapisano %ld B\n", count);
            }
        }
        
        printf("[P] Zapisano %ld B.\n", count);
        close(fd[1]);
        wait(NULL);

    } else {
        close(fd[1]);
        printf("[C] 5s sleep\n");
        sleep(5);
        printf("[C] sleep end\n");

        char bufor[1024];
        long total_read = 0;
        int n;
        
        while ((n = read(fd[0], bufor, sizeof(bufor))) > 0) {
            total_read += n;
            printf("[C] read %d B \n", n);
        }
        
        printf("[C] Read %ld B.\n", total_read);
        close(fd[0]);
        exit(0);
    }
    
    return 0;
}
