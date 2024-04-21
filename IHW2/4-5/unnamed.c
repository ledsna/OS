#include "../hive.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <число пчел>\n", argv[0]);
        return 1;
    }

    int num_bees = atoi(argv[1]);
    if (num_bees <= 3) {
        fprintf(stderr, "Число пчёл должно быть больше трех.\n");
        return 1;
    }

    // Выделение разделяемой памяти
    HiveData *hive = mmap(NULL, sizeof(HiveData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    hive->honey = 0;
    hive->collectors = num_bees - 1;
    hive->guards = 1;

    sem_t mutex, honey_mutex, guard_mutex;
    sem_init(&mutex, 1, 1);
    sem_init(&honey_mutex, 1, 1);
    sem_init(&guard_mutex, 1, 1);

    pid_t pid;
    for (int i = 0; i < num_bees; i++) {
        pid = fork();
        if (pid == 0) {
            bee_routine(hive, &honey_mutex, &guard_mutex, i == 0 ? 1 : 0);
            exit(0);
        }
    }

    pid = fork();
    if (pid == 0) {
        winnie_routine(hive, &mutex);
        exit(0);
    }

    for (int i = 0; i <= num_bees; i++) {
        wait(NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&honey_mutex);
    sem_destroy(&guard_mutex);

    munmap(hive, sizeof(HiveData));
    return 0;
}
