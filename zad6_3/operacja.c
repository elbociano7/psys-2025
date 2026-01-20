#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 283811

void sem_lock(int semid) {
	struct sembuf sb = {0, -1, 0};
	semop(semid, &sb, 1);
}

void sem_unlock(int semid) {
	struct sembuf sb = {0, 1, 0};
	semop(semid, &sb, 1);
}

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Usage: %s <operation_count> <operation_value>\n", argv[0]);
        return 1;
    }

    const size_t SHM_SIZE = sizeof(int);

    int shmid = shmget(SHM_KEY, SHM_SIZE, 0); 
    
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
		} else if(pid == 0) {
            printf("child %d\n", i);
            void* shm_ptr = shmat(shmid, NULL, 0);
            // cast do void* poniewaz taki jest typ zwracany przez shmat()
            if(shm_ptr == (void*)-1) {
                perror("shmat error");
                exit(1);
            }

			int* bank_value = (int*) shm_ptr;

			//Wersja najbezpieczniejsza (praktycznie nie powoduje bledow bo jest to bardzo szybka operacja)
            
			//*bank_value += value;			

			//Wersja z wymuszonymi bledami do zaprezentowania
			
			int temp_bank_value = *bank_value + value;

			usleep(1000);

			*bank_value = temp_bank_value;

			//Powrot do "normalnego" kodu	
				
			printf("[%d] (%d) Balance: %d\n", i, value, *bank_value);


            if(shmdt(shm_ptr) == -1) {
                perror("shmdt error");
                exit(2);
            }

            printf("Child %d end.\n", i);
            exit(0);
        } else {
            printf("Fork error");
            exit(1);
        }
    }
   
	for(int j = 0; j < num; j++) {
		wait(NULL);
	}

    printf("Parent exit\n");
    return 0;
}
