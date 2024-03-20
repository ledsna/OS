#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_SEND "/tmp/fifo_send"
#define FIFO_RECEIVE "/tmp/fifo_receive"
#define BUFFER_SIZE 128

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Попытка создания FIFO, игнорируем ошибку, если FIFO уже существует
    mkfifo(FIFO_SEND, 0666);
    mkfifo(FIFO_RECEIVE, 0666);

    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd < 0) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    int sendFd = open(FIFO_SEND, O_WRONLY);
    if (sendFd < 0) {
        perror("Failed to open FIFO_SEND for writing");
        exit(EXIT_FAILURE);
    }

    int receiveFd = open(FIFO_RECEIVE, O_RDONLY);
    if (receiveFd < 0) {
        perror("Failed to open FIFO_RECEIVE for reading");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    // Чтение и отправка данных в FIFO_SEND
    while ((bytesRead = read(inputFd, buffer, BUFFER_SIZE)) > 0) {
        write(sendFd, buffer, bytesRead);
    }
    close(sendFd); // Закрытие канала отправки сообщает receiver о конце передачи

    // Чтение обработанных данных из FIFO_RECEIVE и запись в выходной файл
    int outputFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outputFd < 0) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    while ((bytesRead = read(receiveFd, buffer, BUFFER_SIZE)) > 0) {
        write(outputFd, buffer, bytesRead);
    }

    close(inputFd);
    close(receiveFd);
    close(outputFd);

    return 0;
}
