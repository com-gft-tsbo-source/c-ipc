#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const char * semName = "abcdefg";

int main(int argc, char *argv[])
{
    sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 0);

    if (sem_id == SEM_FAILED){
        perror("producer: [sem_open] failed!\n");
        return 1;
    }

    printf("producer: I am done! Release Semaphore\n");

    if (sem_post(sem_id) < 0)
        printf("producer: [sem_post] failed!\n");

    return 0;
}