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
    if (argc < 5)
    {
        printf("Usage: %s <source_account> <dest_account> <operation_count> <operation_value>\n", argv[0]);
        exit(1);
    }

    int source_bank_id = atoi(argv[1]);
    int dest_bank_id = atoi(argv[2]);
    int num = atoi(argv[3]);
    int value = atoi(argv[4]);

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
            struct account source_ac = use_account(source_bank_id);
            struct account dest_ac = use_account(dest_bank_id);

            printf("attempting to lock\n");
            int *source_balance = lock_account(&source_ac);
            int *dest_balance = lock_account(&dest_ac);
            printf("locked\n");

            // Wersja bezpieczniejsza
            // transfer(source_balance, dest_balance, value);

            // Wersja mniej bezpieczna (z dodatkowym usleep) aby zwiekszyc szanse
            // na race condition (ktore nie wystapi poniewaz sa uzywane semafory)
            slow_transfer(source_balance, dest_balance, value, 5000);

            printf("[%d] (%d) Balance: source: %d; destination: %d\n", i, value, *source_balance, *dest_balance);

            printf("unlocking\n");
            unlock_account(&source_ac);
            unlock_account(&dest_ac);

            printf("detatching\n");
            detatch_account(&source_ac);
            detatch_account(&dest_ac);

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
