#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#define QTD_MEN 128
#define TAM_BUFFER 8

int main(int argc, char *argv[]) {
    int segmento;
    pid_t pidProcesso1, pidProcesso2;

    // Alocar memória compartilhada
    segmento = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


    /* Processo 1 - Envio de mensagens */
    pidProcesso1 = fork();
    if (pidProcesso1 == 0) {
        for (int i = 0; i < QTD_MEN; i++) {
            // Enviar mensagem
        }

    } else {
        print("\nErro ao criar Processo 1.\n");
        exit(1);
    }

    /* Processo 2 - Leitura de mensagens */
    pidProcesso2 = fork();
    if (pidProcesso2 == 0) {
        for (int i = 0; i < QTD_MEN; i++) {
            // Ler mensagem
        }

    } else {
        print("\nErro ao criar Processo 2.\n");
        exit(1);
    }
    return 0;
}