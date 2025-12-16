#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int p_log(char *from, char *msg){
    printf("%s[%d]: %s\n", from, getpid(), msg);
}

int main(void) {
    int fd[2];
    const char *wiadomosc = "Wiadomosc od rodzica do dziecka!";
    char bufor[1024];

    if(pipe(fd) == -1) {
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid > 0) {
        // Dziecko
        printf("[D] Start \n");
        close(fd[1]);
        int n = read(fd[0], bufor, sizeof(bufor));
        if(n == -1) {
            printf("[D] Blad");
        }
        bufor[n] = '\0';
        printf("[D] Odczytane: ", bufor, "\n");
    } else if(pid == 0) {
        // Rodzic
        printf("[R] Start \n");
        close(fd[0]);
        write(fd[1], wiadomosc, strlen(wiadomosc));
        close(fd[1]);
        wait(NULL);
    } else {
        printf("Wystapil blad dzielenia programu");
    }
}
