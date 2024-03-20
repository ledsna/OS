#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "../converter.h"

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <входной файл> <выходной файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Ошибка при создании канала");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == -1) {
        perror("Ошибка при создании процесса");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Дочерний процесс 1: чтение из файла и передача данных
        close(pipe1[0]); // Закрыть неиспользуемый конец чтения
        FILE *inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            perror("Ошибка при открытии входного файла");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        int bytesRead = fread(buffer, 1, sizeof(buffer), inputFile);
        fclose(inputFile);
        write(pipe1[1], buffer, bytesRead);
        close(pipe1[1]);
        exit(EXIT_SUCCESS);
    } else {
        pid2 = fork();
        if (pid2 == -1) {
            perror("Ошибка при создании процесса");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            // Дочерний процесс 2: получение, обработка данных и передача далее
            close(pipe1[1]); // Закрыть неиспользуемый конец записи в первом канале
            close(pipe2[0]); // Закрыть неиспользуемый конец чтения во втором канале
            char buffer[BUFFER_SIZE];
            int bytesRead = read(pipe1[0], buffer, sizeof(buffer));
            buffer[bytesRead] = '\0';
            convertConsonantsToUpper(buffer);
            write(pipe2[1], buffer, strlen(buffer) + 1);
            close(pipe1[0]);
            close(pipe2[1]);
            exit(EXIT_SUCCESS);
        } else {
            // Родительский процесс: получение обработанных данных и запись в файл
            close(pipe1[0]); // Закрыть первый канал полностью
            close(pipe1[1]);
            close(pipe2[1]); // Закрыть неиспользуемый конец записи во втором канале
            FILE *outputFile = fopen(argv[2], "w");
            if (outputFile == NULL) {
                perror("Ошибка при открытии выходного файла");
                exit(EXIT_FAILURE);
            }

            char buffer[BUFFER_SIZE];
            int bytesRead = read(pipe2[0], buffer, sizeof(buffer));
            fwrite(buffer, 1, bytesRead, outputFile);
            fclose(outputFile);
            close(pipe2[0]);
        }
    }

    return EXIT_SUCCESS;
}
