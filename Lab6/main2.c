#include "message.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main() {
    key_t key;
    int msgid;
    pid_t pid;
	int status;
    struct msqid_ds msq;

    msq.msg_perm.mode = 0666;
    msq.msg_qbytes = 32; // Definir número máximo de bytes na fila de mensagens


    // Criar a fila de mensagens
    msgid = msgget(55, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // Modificar as informações da fila de mensagens
    if (msgctl(msgid, IPC_SET, &msq) == -1) {
        perror("msgctl");
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
        processo2_asssinc(msgid);
    } else {
        // Processo pai (Processo 1)
        processo1_asssinc(msgid);

		// Aguardar o processo filho terminar
		waitpid(pid, &status, 0);

		// Remover a fila de mensagens
		if (msgctl(msgid, IPC_RMID, NULL) == -1) {
			perror("msgctl");
			exit(1);
		}
    }

    return 0;
}
