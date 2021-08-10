#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const char *semName = "asdfsd";

int main(int argc, char *argv[])
{
    sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 0);

    if (sem_id == SEM_FAILED){
        perror("consumer: [sem_open] failed!\n"); 
        return 1;
    }

    printf("consumer: wait for producer to trigger us.\n");

    if (sem_wait(sem_id) < 0){
        printf("consumer: [sem_wait] failed!\n");
        return 2;
    }

    printf("consumer: triggered!\n");
    
    if (sem_close(sem_id) != 0){
        perror("consumer: [sem_close] failed!\n");
        return 3;
    }

    if (sem_unlink(semName) < 0){
        printf("consumer: [sem_unlink] failed!\n");
        return 4;
    }

    return 0;
}