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
	int msgid;
	pid_t pidProcesso1;

	key = ftok("key", 65);
	msgid = msgget(key, 0666 | IPC_CREAT);
	// Alocar memória compartilhada
	// segmento = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	/* Processo 1 - Envio de mensagens */
	pidProcesso1 = fork();
	if (pidProcesso1 == -1)
	{
		printf("\nErro ao criar Processo 1.\n");
		exit(1);
	}
	if (pidProcesso1 == 0)
	{
		// Enviar mensagem
		process1_sync(msgid);
	}
	else
	{
		// Enviar mensagem
		process2_sync(msgid);
	}

	return 0;
}