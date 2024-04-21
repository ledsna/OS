#include "hive.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

void bee_routine(HiveData *hive, sem_t *honey_mutex, sem_t *guard_mutex, int bee_type) {
    while (1) {
        sleep(rand() % 3 + 1); // Случайная задержка, имитирующая работу

        if (bee_type == 0) { // Пчела-сборщик мёда
            sem_wait(honey_mutex);
            if (hive->honey < MAX_HONEY) {
                hive->honey++;
                printf("Пчела добавила мёд. Всего мёда: %d, сборщиков: %d, охранников: %d\n", hive->honey, hive->collectors, hive->guards);
            }
            // Решение стать охранником
            if (rand() % 4 == 0) {
                bee_type = 1;
                hive->collectors--;
                hive->guards++;
                printf("Пчела принесла мёд и решила охранять улей. Всего мёда: %d, сборщиков: %d, охранников: %d\n", hive->honey, hive->collectors, hive->guards);
            }
            sem_post(honey_mutex);
        } else { // Пчела-охранник
            sem_wait(guard_mutex);
            // Решение собирать мёд
            if (rand() % (hive->guards) >= 1 && hive->guards > 1) { // Чем больше охранников, тем с большей вероятностью один из них пойдёт собирать мёд
                bee_type = 0;
                hive->guards--;
                hive->collectors++;
                printf("Пчела устала охранять улей и полетела собирать мёд. Всего мёда: %d, сборщиков: %d, охранников: %d\n", hive->honey, hive->collectors, hive->guards);
            }
            sem_post(guard_mutex);
        }
    }
}

void winnie_routine(HiveData *hive, sem_t *hive_mutex) {
    while (1) {
        sleep(5);

        sem_wait(hive_mutex);
        if (hive->honey >= WAKEUP_THRESHOLD) {
            if (hive->guards < 3) {
                printf("Винни-Пух забирает весь мёд!\n");
                hive->honey = 0;
            } else {
                printf("Винни-Пух отбит охранниками!\n");
            }
        }
        sem_post(hive_mutex);
    }
}
