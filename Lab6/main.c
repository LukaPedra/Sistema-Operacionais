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

int main(int argc, char *argv[])
{
	key_t key;
	int msgid, status;
	pid_t pid;

	key = ftok("key", 7);
	msgid = msgget(key, 0666 | IPC_CREAT);

	/* Processo 1 - Envio de mensagens */
	pid = fork();
	if (pid == 0)
	{
		// Enviar mensagem
		processo1_sinc(msgid);

	} else if (pid < 0) {
        perror("\nErro ao criar Processo 1.\n");
        exit(-1);
    }

	/* Processo 2 - Leitura de mensagens */
	else {
		// Enviar mensagem
		processo2_sinc(msgid);
	} 

	/* Espera dos processos filhos */
	wait(&status);
	
	// Remover a fila de mensagens
    msgctl(msgid, IPC_RMID, NULL) ;

	return 0;
}