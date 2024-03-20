#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "../converter.h"


#define BUFFER_SIZE 5000
#define FIFO1 "/tmp/fifo_task7_1"
#define FIFO2 "/tmp/fifo_task7_2"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <входной файл> <выходной файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Создаем именованные каналы
    if (mkfifo(FIFO1, 0666) == -1 || mkfifo(FIFO2, 0666) == -1) {
        perror("Ошибка создания FIFO");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка создания процесса");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        int fifo1 = open(FIFO1, O_RDONLY);
        int fifo2 = open(FIFO2, O_WRONLY);
        char buffer[BUFFER_SIZE];

        int bytesRead = read(fifo1, buffer, BUFFER_SIZE); // Чтение данных из первого FIFO
        buffer[bytesRead] = '\0';
        convertConsonantsToUpper(buffer); // Обработка данных

        write(fifo2, buffer, strlen(buffer) + 1); // Отправка обработанных данных обратно во второй FIFO
        close(fifo1);
        close(fifo2);
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        int fifo1 = open(FIFO1, O_WRONLY);
        int fifo2 = open(FIFO2, O_RDONLY);

        FILE *inputFile = fopen(argv[1], "r");
        if (!inputFile) {
            perror("Ошибка при открытии входного файла");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        int bytesRead = fread(buffer, 1, BUFFER_SIZE, inputFile); // Чтение из файла
        write(fifo1, buffer, bytesRead); // Отправка данных в первый FIFO
        fclose(inputFile);
        close(fifo1);

        FILE *outputFile = fopen(argv[2], "w");
        if (!outputFile) {
            perror("Ошибка при открытии выходного файла");
            exit(EXIT_FAILURE);
        }

        bytesRead = read(fifo2, buffer, BUFFER_SIZE); // Чтение обработанных данных из второго FIFO
        fwrite(buffer, 1, bytesRead, outputFile); // Запись в файл
        fclose(outputFile);
        close(fifo2);

        // Удаляем именованные каналы
        unlink(FIFO1);
        unlink(FIFO2);
    }

    return EXIT_SUCCESS;
}
