#include "message.h"

int main() {
    key_t key;
    int msgid;
    pid_t pid;
	int status;

    // Gerar uma chave única
    key = ftok("progfile", 65);

    // Criar a fila de mensagens
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // Criar um novo processo
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Processo filho (Processo 2)
        process2_async(msgid);
    } else {
        // Processo pai (Processo 1)
        process1_async(msgid);

		// Aguardar o processo filho terminar
		waitpid(pid, &status, 0);
		printf("JABBAAAAA\n");

		// Remover a fila de mensagens
		if (msgctl(msgid, IPC_RMID, NULL) == -1) {
			perror("msgctl");
			exit(1);
		}

    }

    

    return 0;
}
