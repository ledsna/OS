#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t bit = 0;
volatile int number = 0;
volatile int bit_count = 0;
pid_t sender_pid;

void handle_signal(int signum, siginfo_t *info, void *context) {
    // Игнорируем сигнал, если он не от указанного отправителя
    if (info->si_pid != sender_pid) {
        return;
    }

    // SIGUSR1 - это 0, SIGUSR2 - это 1
    bit = (signum == SIGUSR2) ? 1 : 0;
    number |= (bit << bit_count++);

    // Сброс после получения достаточного количества битов
    if (bit_count == sizeof(int) * 8) {
        printf("Received number: %d\n", number);
        bit_count = 0;
        number = 0; // Обнуляем число для следующего приема
    }
}

int main() {
    // Выводим PID этой программы-приёмника
    printf("Receiver PID: %d\n", getpid());

    // Настраиваем обработчик сигнала
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_signal;
    sigemptyset(&sa.sa_mask);

    // Устанавливаем обработчики для SIGUSR1 и SIGUSR2
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    // Запрашиваем PID отправителя у пользователя
    printf("Введите PID отправителя: ");
    scanf("%d", &sender_pid);

    printf("Ожидание данных от PID %d...\n", sender_pid);

    // Вечный цикл, ожидание сигналов
    while (1) {
        pause(); // Ожидание сигнала
    }

    return 0; // Эта строка недостижима
}
