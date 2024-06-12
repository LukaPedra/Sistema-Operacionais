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
#include "message.h"

#define QTD_MEN 128
#define TAM_BUFFER 8

int main(int argc, char *argv[])
{
	key_t key;
	int msgid, status;
	pid_t pidProcesso1, pidProcesso2;

	key = ftok("key", 65);
	msgid = msgget(key, 0666 | IPC_CREAT);

	/* Processo 1 - Envio de mensagens */
	pidProcesso1 = fork();
	if (pidProcesso1 == 0)
	{
		// Enviar mensagem
		process1_sync(msgid);

	} else if (pidProcesso1 < 0) {
        perror("\nErro ao criar Processo 1.\n");
        exit(-1);
    }

	/* Processo 2 - Leitura de mensagens */
	pidProcesso2 = fork();
	if (pidProcesso2 == 0) {
		// Enviar mensagem
		process2_sync(msgid);
	} else if (pidProcesso2 < 0) {
		perror("\nErro ao criar Processo 2.\n");
		exit(-1);
	}

	/* Espera dos processos filhos */
	for (int i = 0; i < 2; i++) {
        wait(&status);
    }

	return 0;
}