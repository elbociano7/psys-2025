#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "bank.h"

int shmid;
int sem_id;

void cleanup()
{
    if (sem_id > 0)
    {
        semctl(sem_id, 0, IPC_RMID, NULL);
    }
    if (shmid > 0)
    {
        shmctl(shmid, IPC_RMID, NULL);
    }
    printf("SIGINT exit\n");
    exit(0);
}

int main(int argc, char **argv)
{

    signal(SIGINT, cleanup);

    if (argc < 3)
    {
        printf("Usage: %s <bank_id> <initial_value>\n", argv[0]);
        exit(1);
    }

    int bank_id = atoi(argv[1]);
    sem_id = bank_id + 1;

    shmid = shmget(bank_id, BANK_SHM_SIZE, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("SHM open fail");
        exit(2);
    }

    int semid = semget(bank_id, 1, IPC_CREAT | 0666);

    semctl(semid, 0, SETVAL, 1);

    int *balance = (int *)shmat(shmid, NULL, 0);

    printf("SHM Attatched\n");

    *balance = atoi(argv[2]);

    while (1)
    {
        lock_sem(sem_id);
        printf("Balance: %d\n", *balance);
        unlock_sem(sem_id);
        sleep(2);
    }
    return 0;
}
