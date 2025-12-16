#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFF_SIZE 1024

char* msg1 = "hello, world #1";
char* msg2 = "hello, world #2";
char* msg3 = "hello, world #3";

int main() {
    int fd[2], n;
    char buffer[BUFF_SIZE];

    if (pipe(fd) == -1) {
        exit(EXIT_FAILURE);         
    }

    pid_t pid = fork();

    if(pid == -1) {
        exit(EXIT_FAILURE);
    }

    if(pid > 0) {
        write(fd[1], msg1, strlen(msg1));
        wait(NULL);
        close(fd[1]);
        printf("[P] Child end\n");
    } else {
        while(n = read(fd[0], buffer, BUFF_SIZE) > 0) {
            printf("%s/n", buffer);
        }
        if(n != 0){
            exit(2);
        }
    }
}
