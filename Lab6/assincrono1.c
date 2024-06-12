#include "message.h"

void process1_async(int msgid) {
    struct message msg;
    for (int i = 1; i <= 128; i++) {
        msg.msg_type = 1;
        msg.msg_data = i;
        if (msgsnd(msgid, &msg, sizeof(msg.msg_data), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        printf("Processo 1: Mensagem %d enviada\n", i);

        // Se o buffer estiver cheio, aguarde até que haja espaço
        if ((i % 8) == 0) {
            sleep(1); // simular espera
        }
    }
}