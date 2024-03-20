#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../converter.h"

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <входной файл> <выходной файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipe1[2], pipe2[2]; // Создаем два канала: один для чтения, другой для записи
    pid_t pid;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Ошибка создания канала");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("Ошибка создания процесса");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        close(pipe1[1]); // Закрыть неиспользуемый конец записи
        close(pipe2[0]); // Закрыть неиспользуемый конец чтения

        char buffer[BUFFER_SIZE];
        int bytesRead = read(pipe1[0], buffer, BUFFER_SIZE); // Чтение данных от родителя
        buffer[bytesRead] = '\0';
        convertConsonantsToUpper(buffer); // Обработка данных

        write(pipe2[1], buffer, strlen(buffer) + 1); // Отправка обработанных данных обратно родителю
        close(pipe1[0]);
        close(pipe2[1]);
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        close(pipe1[0]); // Закрыть неиспользуемый конец чтения
        close(pipe2[1]); // Закрыть неиспользуемый конец записи

        FILE *inputFile = fopen(argv[1], "r");
        if (!inputFile) {
            perror("Ошибка при открытии входного файла");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        int bytesRead = fread(buffer, 1, BUFFER_SIZE, inputFile);
        write(pipe1[1], buffer, bytesRead); // Отправка данных дочернему процессу
        fclose(inputFile);
        close(pipe1[1]);

        FILE *outputFile = fopen(argv[2], "w");
        if (!outputFile) {
            perror("Ошибка при открытии выходного файла");
            exit(EXIT_FAILURE);
        }

        bytesRead = read(pipe2[0], buffer, BUFFER_SIZE); // Чтение обработанных данных от дочернего процесса
        fwrite(buffer, 1, bytesRead, outputFile);
        fclose(outputFile);
        close(pipe2[0]);
    }

    return EXIT_SUCCESS;
}
