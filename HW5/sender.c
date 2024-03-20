#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

pid_t receiver_pid;
volatile sig_atomic_t confirmation_received = 0;

void handle_confirmation(int signum) {
    // Подтверждение получено
    confirmation_received = 1;
}

void send_bit(int bit) {
    confirmation_received = 0; // Сброс подтверждения
    if (bit == 0) {
        kill(receiver_pid, SIGUSR1);
    } else {
        kill(receiver_pid, SIGUSR2);
    }

    // Ожидание подтверждения
    while (!confirmation_received) {
        pause(); // Ожидание сигнала подтверждения
    }
}

int main() {
    printf("Sender PID: %d\n", getpid());

    printf("Enter receiver PID: ");
    scanf("%d", &receiver_pid);

    // Установка обработчика подтверждений
    struct sigaction sa;
    sa.sa_handler = handle_confirmation;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    int number;
    printf("Enter number: ");
    scanf("%d", &number);

    int bit_count = sizeof(int) * 8;

    // Отправка битов
    for (int i = 0; i < bit_count; i++) {
        send_bit((number >> i) & 1);
    }

    // Отправка сигнала об окончании передачи (можно использовать SIGUSR1)
    kill(receiver_pid, SIGUSR1);
    return 0;
}
