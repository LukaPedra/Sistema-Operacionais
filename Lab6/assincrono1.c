#include "message.h"

void processo1_asssinc(int msgid) {
    struct message msg;
    for (int i = 1; i <= QTD_MEN; i++) {
        msg.msg_type = 1;
        msg.msg_data = i;
        if (msgsnd(msgid, &msg, sizeof(msg.msg_data), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        printf("Processo 1: Mensagem %d enviada\n", i);
    }
}