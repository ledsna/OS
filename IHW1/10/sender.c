#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // Создание очереди сообщений
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

    if (msgid == -1) {
        fprintf(stderr, "msgget failed\n");
        exit(EXIT_FAILURE);
    }

    // Чтение и отправка данных
    while(running) {
        printf("Enter some text: ");
        fgets(some_data.text, MAX_TEXT, stdin);
        some_data.my_msg_type = 1;

        if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }

        if (strncmp(some_data.text, "end", 3) == 0) {
            running = 0;
        }
    }

    exit(EXIT_SUCCESS);
}
