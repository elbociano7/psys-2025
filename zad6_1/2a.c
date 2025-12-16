#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int potok[2];
    pid_t pid1, pid2;

    if (argc < 3) {
        printf("Uzycie: %s cmd1 cmd2 [cmd2_arg1...]\n", argv[0]);
        return 1;
    }

    if (pipe(potok) == -1) {
        perror("pipe");
        return 1;
    }

    if ((pid1 = fork()) == 0) {
        close(potok[0]); 

        dup2(potok[1], STDOUT_FILENO); 
        close(potok[1]);

        execlp(argv[1], argv[1], NULL);
        perror("execlp cmd1");
        exit(1);
    }
    
    if ((pid2 = fork()) == 0) {
        close(potok[1]); 

        dup2(potok[0], STDIN_FILENO); 
        close(potok[0]);

        execvp(argv[2], &argv[2]);
        perror("execvp cmd2");
        exit(1);
    }

    close(potok[0]);
    close(potok[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
