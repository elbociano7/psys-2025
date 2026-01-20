#ifndef BANK_ACCOUNT
#define BANK_ACCOUNT

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <sys/shm.h>

// rozmiar typu int
#define BANK_SHM_SIZE 4

struct account
{
    int shm_id;
    int sem_id;
    int *shm;
    bool locked;
};

void lock_sem(int sem_id)
{
    struct sembuf sb = {0, -1, 0};
    semop(sem_id, &sb, 1);
}

void unlock_sem(int sem_id)
{
    struct sembuf sb = {0, 1, 0};
    semop(sem_id, &sb, 1);
}

struct account use_account(int bank_id)
{
    int shm_id = shmget(bank_id, BANK_SHM_SIZE, 0);
    if (shm_id < 0)
    {
        printf("SHMget error");
        exit(1);
    }

    void *shm = shmat(shm_id, NULL, 0);
    if (shm == (void *)-1)
    {
        printf("SHM error");
        exit(1);
    }

    int sem_id = semget(bank_id, 1, 0);
    if (sem_id < 0)
    {
        printf("SEM error");
        exit(1);
    }

    struct account a = {shm_id, sem_id, (int *)shm, false};
    return a;
}

int *lock_account(struct account *a)
{
    lock_sem(a->sem_id);
    a->locked = true;
    return a->shm;
}

void unlock_account(struct account *a)
{
    if (a->locked)
    {
        unlock_sem(a->sem_id);
        a->locked = false;
    }
}

void detatch_account(struct account *a)
{
    unlock_account(a);
    if (shmdt(a->shm) == -1)
    {
        printf("SHM detatch error");
        exit(1);
    }
}

#endif
