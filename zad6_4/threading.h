#ifndef _BANK_THREADING
#define _BANK_THREADING

#include <complex.h>
#include <math.h>
#include <string.h>
#include <sys/_pthread/_pthread_t.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int thread_count;
pthread_t *threads = NULL;

void create_thread(pthread_t *thread, void *(*thread_job)(void *), void *thread_arg)
{
    if (pthread_create(thread, NULL, thread_job, thread_arg) != 0)
    {
        printf("Threading error");
        exit(1);
    }
}

void run_workers(void *(*worker_job)(void *), void *worker_job_data, int count)
{

    thread_count += count;

    if (threads != NULL)
    {
        threads = realloc(threads, sizeof(pthread_t) * thread_count);
    }
    else
    {
        threads = malloc(sizeof(pthread_t) * thread_count);
    }

    for (int i = 0; i < count; i++)
    {
        create_thread(&threads[i], worker_job, worker_job_data);
    }
}

void clean_threads(bool kill)
{
    if (threads != NULL)
    {
        if(kill == true) {
            printf("Cleaning up %d threads\n", thread_count);
        } else {
            printf("Waiting for %d threads to end\n", thread_count);
        }
        

        int j = thread_count;
        while (--j >= 0)
        {
            if (kill)
            {
                pthread_kill(threads[j], 0);
            }
            else
            {
                pthread_join(threads[j], NULL);
            }
        }

        free(threads);
        threads = NULL;
    } else if(thread_count == 0) {
        printf("Thread cleanup error");
        exit(1);
    }
}

#endif
