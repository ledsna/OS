#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void send_bit(pid_t pid, int bit) {
    if (bit == 0) {
        kill(pid, SIGUSR1);
    } else {
        kill(pid, SIGUSR2);
    }
}

int main() {
    // Выводим PID этой программы-отправителя
    printf("Sender PID: %d\n", getpid());

    pid_t pid;
    int number;

    printf("Enter receiver PID: ");
    scanf("%d", &pid);

    printf("Enter number: ");
    scanf("%d", &number);

    int bit_count = sizeof(int) * 8;

    for (int i = 0; i < bit_count; ++i) {
        send_bit(pid, (number >> i) & 1);
        usleep(100000); // Sleep for 100ms to prevent signal overlap
    }

    return 0;
}
