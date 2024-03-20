#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../converter.h"

#define BUFFER_SIZE 5000
#define FIFO_NAME "/tmp/my_fifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <выходной файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fifoFd = open(FIFO_NAME, O_RDONLY);
    if (fifoFd == -1) {
        perror("Ошибка открытия FIFO");
        exit(EXIT_FAILURE);
    }

    FILE *outputFile = fopen(argv[1], "w");
    if (!outputFile) {
        perror("Ошибка открытия выходного файла");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = read(fifoFd, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytesRead] = '\0';
        convertConsonantsToUpper(buffer);
        fputs(buffer, outputFile);
    }

    close(fifoFd);
    fclose(outputFile);

    return EXIT_SUCCESS;
}
