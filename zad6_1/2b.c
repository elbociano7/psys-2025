#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMDS 20
#define MAX_ARGS 20

int main(int argc, char *argv[]) {
    if (argc < 3 || argc % 2 == 0) {
        printf("Uzycie: %s cmd1 arg1 cmd2 arg2 ...\n", argv[0]);
        return 1;
    }

    char *cmds[MAX_CMDS][MAX_ARGS];
    int cmdcount = 0;
    int i;

    for (i = 1; i < argc; i += 2) {
        if (cmdcount >= MAX_CMDS) {
            fprintf(stderr, "Za duzo polecen.\n");
            return 1;
        }
        cmds[cmdcount][0] = argv[i];
        cmds[cmdcount][1] = argv[i + 1];
        cmds[cmdcount][2] = NULL;

        cmdcount++;
    }

    if (cmdcount == 0) {
        printf("Brak polecen.\n");
        return 1;
    }

    int pipes[MAX_CMDS][2];
    for (i = 0; i < cmdcount - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    pid_t pids[MAX_CMDS];

    for (i = 0; i < cmdcount; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            if (i > 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 STDIN");
                    exit(1);
                }
            }
            if (i < cmdcount - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 STDOUT");
                    exit(1);
                }
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
