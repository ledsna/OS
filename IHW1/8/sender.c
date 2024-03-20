#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 5000
#define FIFO_NAME "/tmp/my_fifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <входной файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Создаем именованный канал, если он еще не существует
    mkfifo(FIFO_NAME, 0666);

    int inputFile = open(argv[1], O_RDONLY);
    if (inputFile == -1) {
        perror("Ошибка открытия входного файла");
        exit(EXIT_FAILURE);
    }

    int fifoFd = open(FIFO_NAME, O_WRONLY);
    if (fifoFd == -1) {
        perror("Ошибка открытия FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = read(inputFile, buffer, BUFFER_SIZE)) > 0) {
        write(fifoFd, buffer, bytesRead);
    }

    close(inputFile);
    close(fifoFd);

    return EXIT_SUCCESS;
}
