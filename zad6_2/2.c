#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define FIFO_PATH "/tmp/mplayer_control_fifo"

int main(int argc, char *argv[]) {
    int fd;
    char command[128];
    char ch;

    if (mkfifo(FIFO_PATH, 0600) == -1) {
        if (errno != EEXIST) {
            perror("Błąd przy tworzeniu fifo");
            exit(EXIT_FAILURE);
        }
    }

    fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Błąd otwarcia potoku");
        unlink(FIFO_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Połączono z mplayerem\n");
    printf("Komendy:\n 'p' - pauza\n 'f' - do przodu 5s\n 'b' - do tyłu 5s\n 'q' - wyjście\n");

    while (1) {
        ch = getchar();
        if(ch == '\n') continue;

        if (ch == 'p') {
            snprintf(command, sizeof(command), "pause\n");
            printf("Wysłano: pause\n");
        } 
        else if (ch == 'f') {
            snprintf(command, sizeof(command), "seek 5 0\n"); 
            printf("Wysłano: seek +5s\n");
        } 
        else if (ch == 'b') {
            snprintf(command, sizeof(command), "seek -5 0\n");
            printf("Wysłano: seek -5s\n");
        } 
        else if (ch == 'q') {
            snprintf(command, sizeof(command), "quit\n");
            write(fd, command, strlen(command));
            printf("Kończenie pracy...\n");
            break;
        } 
        else {
            printf("Nieznana komenda: %c\n", ch);
            continue;
        }

        if (write(fd, command, strlen(command)) == -1) {
            perror("Błąd zapisu do FIFO");
            break;
        }
    }

    close(fd);
    if (unlink(FIFO_PATH) == -1) {
        perror("Błąd usuwania FIFO");
    } else {
        printf("Potok został usunięty.\n");
    }

    return 0;
}
