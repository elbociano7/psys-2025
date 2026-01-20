#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <time.h>
#include "threading.h"

struct bank {
    int balance;
    sem_t* sem;
};

struct operation_data_t
{
    struct bank* bank;
    int amount;
};

struct transfer_data_t
{
    struct bank* source_bank;
    struct bank* dest_bank;
    int amount;
};

struct bank bank1 = {0, 0};
struct bank bank2 = {0, 0};

bool use_sem = false;

int last_op_id = 0;

int get_id() {
    return last_op_id++;
}

void random_sleep() {
    struct timespec ts;
    ts.tv_sec = 0;
    // przy max_rand = 2^32
    // rand()[0-2^32] * 5 aby przekroczylo 1000000000
    // to daje 0-1s
    ts.tv_nsec = (long)rand() * 5L % 1000000000L;
    nanosleep(&ts, NULL);
}

void* operation(void* data_p) {
    int id = get_id();
    // W przypadku uzycia bez semaforow gdy wszystkie operacje wystartowaly
    // w tym samym momencie mialy one ta sama wartosc poczatkowa
    // (bylo zbyt nudno) dlatego teraz na poczatku jest losowy czas oczekiwania
    // od 0 do 1s
    random_sleep();
    struct operation_data_t* data = data_p;
    printf("Operation %d dispatched: %d\n", id, data->amount);


    if(use_sem) {
        sem_wait(data->bank->sem);
    }

    int temp_balance = data->bank->balance + data->amount;
    random_sleep();
    data->bank->balance = temp_balance;
    printf("Operation %d: %d. Balance: %d\n", id, data->amount, data->bank->balance);

    if(use_sem) {
        sem_post(data->bank->sem);
    }

    return NULL;
}

void* transfer(void* data_p) {
    int id = get_id();
    
    //tu to samo
    random_sleep();
    struct transfer_data_t* data = data_p;
    printf("Transfer %d dispatched: %d\n", id, data->amount);
    
    if(use_sem) {
        // Zapewnienie odpowiedniej kolejnosci blokowania semaforow
        // aby uniknac zakleszczenia na wypadek funkcjonalnosci przelewow odwrotnych
        if (&data->source_bank->sem < &data->dest_bank->sem) {
            sem_wait(data->source_bank->sem);
            sem_wait(data->dest_bank->sem);
        } else {
            sem_wait(data->dest_bank->sem);
            sem_wait(data->source_bank->sem);
        }
    }

    int temp_balance_1 = data->source_bank->balance - data->amount;
    int temp_balance_2 = data->dest_bank->balance + data->amount;
    random_sleep();
    data->source_bank->balance = temp_balance_1;
    data->dest_bank->balance = temp_balance_2;
    printf("Transferred %d (Transfer %d). Balances: Source: %d; Dest: %d;\n", data->amount, id, data->source_bank->balance, data->dest_bank->balance);

    if(use_sem) {
        // Odblokowanie semaforow
        sem_post(data->source_bank->sem);
        sem_post(data->dest_bank->sem);
    }
    return NULL;
}

void clean_sem() {
    sem_close(bank1.sem);
    sem_close(bank2.sem);
    sem_unlink("/bank1_sem");
    sem_unlink("/bank2_sem");
}

void cleanup()
{
    clean_threads(true);
    clean_sem();
    printf("SIGINT exit\n");
    exit(0);
}

void read_banks() {
    printf("Balance: 1: %d; 2: %d\n", bank1.balance, bank2.balance);
}

void bank_worker(void *worker_arg) {
    while(1) {
        read_banks();
        sleep(2);
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    signal(SIGINT, cleanup);

    if (argc < 6)
    {
        printf("Usage: %s <initial_value_1> <initial_value_2> <o[peration]|t[ransfer]> <count> <amount> <use_sem:1|0]>\n", argv[0]);
        exit(1);
    }

    if (argc > 6) {
        if (argv[6][0] == '1') {
            use_sem = true;
        }
    }
    if (use_sem == true) {
        printf("Semaphores enabled\n");
    } else {
        printf("Semaphores disabled\n");
    }

    bank1.balance = atoi(argv[1]);
    bank2.balance = atoi(argv[2]);

    // nie dziala na macos :c
    //sem_init(bank1.sem, 0, 1);
    //sem_init(bank2.sem, 0, 1);
    
    bank1.sem = sem_open("/bank1_sem", O_CREAT, 0644, 1);
    bank2.sem = sem_open("/bank2_sem", O_CREAT, 0644, 1);

    if(bank1.sem == SEM_FAILED || bank2.sem == SEM_FAILED) {
        printf("Sem open error");
        exit(1);
    }

    int count = atoi(argv[4]);
    int amount = atoi(argv[5]);

    // Nie ma potrzeby odczytywania stanu kont co jakis czas
    // bo nie trzeba czekac na oddzielny program
    // run_workers((void*)bank_worker, NULL, 1);
    read_banks();

    switch(argv[3][0]) {
        default:
            printf("Operation type %s not found\n", &argv[3][0]);
            break;
        case 'o':
            printf("Running %d operation workers...\n", count);
            struct operation_data_t op_data = {&bank1, amount};
            run_workers(operation, &op_data, count);
            break;
        case 't':
            printf("Running %d transfer workers...\n", count);
            struct transfer_data_t tr_data = {&bank1, &bank2, amount};
            run_workers(transfer, &tr_data, count);
            break;
    }
    clean_threads(false);
    read_banks();
    printf("Cleaning up sem1/2\n");
    clean_sem();
    return 0;
}
