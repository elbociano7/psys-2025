#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "bank.h"
#include "operations.h"

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("Usage: %s <account> <operation_count> <operation_value>\n", argv[0]);
        exit(1);
    }

    int bank_id = atoi(argv[1]);
    int num = atoi(argv[2]);
    int value = atoi(argv[3]);

    printf("%d operacji o wartosci %d\n", num, value);

    // na wypadek przekroczenia maksymalnej ilosci procesow dla uzytkownika
    int process_count = 0;

    for (int i = 0; i < num; i++)
    {
        pid_t pid = fork();

        if (pid > 0)
        {
            process_count++;
        }
        else if (pid == 0)
        {
            printf("child %d\n", i);

            printf("getting account\n");
            struct account ac = use_account(bank_id);

            printf("attempting to lock\n");
            int *balance = lock_account(&ac);
            printf("locked\n");

            // Wersja bezpieczniejsza
            // add_amount(balance, value);

            // Wersja mniej bezpieczna (z dodatkowym usleep) aby zwiekszyc szanse
            // na race condition (ktore nie wystapi poniewaz sa uzywane semafory)
            slow_add_amount(balance, value, 5000);

            printf("[%d] (%d) Balance: %d\n", i, value, *balance);

            printf("unlocking\n");
            unlock_account(&ac);

            printf("detatching\n");
            detatch_account(&ac);

            printf("detatched\n");
            printf("Child %d end.\n", i);
            exit(0);
        }
        else
        {
            perror("Fork error");
        }
    }

    for (int j = 0; j < process_count; j++)
    {
        wait(NULL);
    }

    printf("Stopped %d child processes\n", process_count);
    printf("Parent exit\n");
    return 0;
}
