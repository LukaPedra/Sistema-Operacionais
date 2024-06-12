#include <message.h>

void process2_async(int msgid) {
    struct message msg;
    for (int i = 1; i <= 128; i++) {
        if (msgrcv(msgid, &msg, sizeof(msg.msg_data), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Processo 2: Mensagem %d recebida\n", msg.msg_data);
    }
}