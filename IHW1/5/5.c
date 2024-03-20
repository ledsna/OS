#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "../converter.h"

#define BUFFER_SIZE 5000
#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <входной файл> <выходной файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Создание именованных каналов
    if (mkfifo(FIFO1, 0666) == -1 || mkfifo(FIFO2, 0666) == -1) {
        perror("Ошибка при создании FIFO");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка при создании процесса");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс: чтение из файла и передача данных через FIFO1
        int inputFile = open(argv[1], O_RDONLY);
        int fifo1 = open(FIFO1, O_WRONLY);
        char buffer[BUFFER_SIZE];
        int bytesRead = read(inputFile, buffer, BUFFER_SIZE);
        write(fifo1, buffer, bytesRead);
        close(inputFile);
        close(fifo1);
        exit(EXIT_SUCCESS);
    } else {
        pid = fork();
        if (pid == -1) {
            perror("Ошибка при создании процесса");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Второй дочерний процесс: чтение из FIFO1, обработка и передача в FIFO2
            int fifo1 = open(FIFO1, O_RDONLY);
            int fifo2 = open(FIFO2, O_WRONLY);
            char buffer[BUFFER_SIZE];
            int bytesRead = read(fifo1, buffer, BUFFER_SIZE);
            buffer[bytesRead] = '\0';
            convertConsonantsToUpper(buffer);
            write(fifo2, buffer, strlen(buffer) + 1);
            close(fifo1);
            close(fifo2);
            exit(EXIT_SUCCESS);
        } else {
            // Родительский процесс: чтение из FIFO2 и запись в файл
            int fifo2 = open(FIFO2, O_RDONLY);
            int outputFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            char buffer[BUFFER_SIZE];
            int bytesRead = read(fifo2, buffer, BUFFER_SIZE);
            write(outputFile, buffer, bytesRead);
            close(outputFile);
            close(fifo2);

            // Удаление FIFO
            unlink(FIFO1);
            unlink(FIFO2);
        }
    }

    return EXIT_SUCCESS;
}
