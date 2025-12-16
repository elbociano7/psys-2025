#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMDS 20
#define MAX_ARGS 20

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uzycie: %s cmd [args] . cmd [args] ...\n", argv[0]);
        return 1;
    }

    char *cmds[MAX_CMDS][MAX_ARGS];
    int cmdcount = 0;
    int argcount = 0;
    int i;

    //Przetworzenie argumentow
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], ".") == 0) {
            cmds[cmdcount][argcount] = NULL;
            cmdcount++;
            argcount = 0;
        } else {
            cmds[cmdcount][argcount++] = argv[i];
        }
    }

    cmds[cmdcount][argcount] = NULL;
    cmdcount++;


    if (cmdcount == 0) {
        printf("brak polecen\n");
        return 1;
    }

    int pipes[MAX_CMDS][2];
    for (i = 0; i < cmdcount - 1; i++) {
        pipe(pipes[i]);
    }

    pid_t pids[MAX_CMDS];

    for (i = 0; i < cmdcount; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < cmdcount - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < cmdcount - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(cmds[i][0], cmds[i]);
            perror("execvp");
            exit(1);
        }
        pids[i] = pid;
    }

    for (i = 0; i < cmdcount - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (i = 0; i < cmdcount; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
