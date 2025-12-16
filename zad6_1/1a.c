#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    char bufor[100];

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
        
        const char *wiadomosc = "Wiadomosc do potoku...";
        write(fd[1], wiadomosc, strlen(wiadomosc));
        
        close(fd[1]);
        wait(NULL);

    } else {
        close(fd[1]); 
        
        int n = read(fd[0], bufor, sizeof(bufor));
        if (n > 0) {
            bufor[n] = '\0';
            printf("[C] read: \"%s\"\n", bufor);
        }
        
        close(fd[0]);
        exit(0);
    }
    
    return 0;
}
