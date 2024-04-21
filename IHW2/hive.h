#ifndef HIVE_H
#define HIVE_H

#include <semaphore.h>

#define MAX_HONEY 30
#define WAKEUP_THRESHOLD 15

// Общая структура улья
typedef struct {
    int honey;
    int collectors;
    int guards;
} HiveData;

void bee_routine(HiveData *hive, sem_t *honey_mutex, sem_t *guard_mutex, int bee_type);
void winnie_routine(HiveData *hive, sem_t *hive_mutex);

#endif // HIVE_H
