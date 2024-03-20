#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t bit_count = 0;
volatile int number = 0;
pid_t sender_pid;

void handle_signal(int signum, siginfo_t *info, void *context) {
    if (info->si_pid != sender_pid) {
        return; // Игнорируем сигналы от других процессов
    }

    // SIGUSR1 - это 0, SIGUSR2 - это 1
    int bit = (signum == SIGUSR2);
    number |= (bit << bit_count++);

    // Отправляем подтверждение обратно отправителю
    kill(sender_pid, SIGUSR1);
}

void handle_confirmation(int signum, siginfo_t *info, void *context) {
    // Обработка подтверждения окончания передачи
    printf("Received number: %d\n", number);
    number = 0; // Сброс числа для следующей передачи
    bit_count = 0; // Сброс счётчика битов
}

int main() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR2, &sa, NULL);

    // Обработчик подтверждения завершения
    sa.sa_sigaction = handle_confirmation;
    sigaction(SIGUSR1, &sa, NULL);

    printf("Receiver PID: %d\n", getpid());

    printf("Enter sender PID: ");
    scanf("%d", &sender_pid);

    // Бесконечный цикл, ожидание сигналов
    while (1) {
        pause();
    }
}
