#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_PATH "/tmp/bank283811"
#define PROJ_ID 283811

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Usage: %s <initial_value>\n", argv[0]);
        return 1;
    }

    const size_t SHM_SIZE = sizeof(int);

    key_t key = 0;

    do {
        key_t key = ftok(SHM_PATH, PROJ_ID);
        if(key == -1) {

        }
    } while (key == -1);

    int shmid = shmget(key, SHM_SIZE, 0); 
    
    if(shmid == -1) {
        perror("SHM open fail");
        return 3;
    }

    int num = atoi(argv[1]);
    int value = atoi(argv[2]);
    
    printf("%d operacji o wartosci %d\n", num, value);
    
    for (int i = 0; i < num; i++) {
        pid_t pid = fork();

        if(pid > 0) {
            if(i == num - 1) {
                wait(NULL); 
            }
        } else if(pid == 0) {
            printf("child %d\n", i);
            void* shm_ptr = shmat(shmid, NULL, 0);
            
            // cast do void* poniewaz taki jest typ zwracany przez shmat()
            if(shm_ptr == (void*)-1) {
                perror("shmat error");
                return 1;
            }

            int* bank_value = (int*)shm_ptr;
            *bank_value += value;

            if(shmdt(shm_ptr) == -1) {
                perror("shmdt error");
                return 2;
            }

            printf("Child %d end.\n", i);
            return 0;
        } else {
            printf("Fork error");
            return 1;
        }
    }
    
    printf("Parent exit\n");
    return 0;
}
