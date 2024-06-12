#include "message.h"

void process2_sync(int msgid) {
    struct message msg;
    for (int i = 1; i <= QTD_MEN; i++) {
        if (msgrcv(msgid, &msg, sizeof(msg.msg_data), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Processo 2: Mensagem %d recebida\n", msg.msg_data);

        // Enviar a resposta
        msg.msg_type = 2;
        if (msgsnd(msgid, &msg, sizeof(msg.msg_data), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        printf("Processo 2: Resposta %d enviada\n", msg.msg_data);
    }
}
