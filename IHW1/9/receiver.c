#include "../converter.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define FIFO_SEND "/tmp/fifo_send"
#define FIFO_RECEIVE "/tmp/fifo_receive"
#define BUFFER_SIZE 128

int main() {
    mkfifo(FIFO_SEND, 0666);
    mkfifo(FIFO_RECEIVE, 0666);

    int sendFd = open(FIFO_SEND, O_RDONLY);
    if (sendFd < 0) {
        perror("Failed to open FIFO_SEND for reading");
        exit(EXIT_FAILURE);
    }

    int receiveFd = open(FIFO_RECEIVE, O_WRONLY);
    if (receiveFd < 0) {
        perror("Failed to open FIFO_RECEIVE for writing");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE + 1]; // +1 для нуль-терминатора
    ssize_t bytesRead;

    while ((bytesRead = read(sendFd, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytesRead] = '\0';
        convertConsonantsToUpper(buffer);
        write(receiveFd, buffer, bytesRead);
    }

    close(sendFd);
    close(receiveFd);

    return 0;
}
