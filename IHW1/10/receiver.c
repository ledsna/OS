#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../converter.h"

#define MAX_TEXT 128

struct my_msg_st {
    long int my_msg_type;
    char text[MAX_TEXT];
};


int main() {
    int running = 1;
    int msgid;
    struct my_msg_st some_data;
    long int msg_to_receive = 0;

    // Доступ к очереди сообщений
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

    if (msgid == -1) {
        fprintf(stderr, "msgget failed\n");
        exit(EXIT_FAILURE);
    }

    // Прием сообщений
    while(running) {
        if (msgrcv(msgid, (void *)&some_data, MAX_TEXT, msg_to_receive, 0) == -1) {
            fprintf(stderr, "msgrcv failed\n");
            exit(EXIT_FAILURE);
        }

        convertConsonantsToUpper(some_data.text);
        printf("Data Received: %s\n", some_data.text);

        if (strncmp(some_data.text, "end", 3) == 0) {
            running = 0;
        }
    }

    // Удаление очереди сообщений
    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
