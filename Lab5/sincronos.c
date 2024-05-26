#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>

#define MAX_TAM_MEN 15
#define QTD_MEN 128

int main(int argc, char *argv[]) {
    int segmento, pidProcesso1, pidProcesso2;
    char* mensagem;
    sem_t* semafaro;

    // Alocar memória compartilhada
    segmento = shmget (IPC_PRIVATE, MAX_TAM_MEN * sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    // Associar a memória compartilhada ao processo
    mensagem = (char*)shmat(segmento, 0, 0);

    // Criação do semáforo
    semafaro = sem_open("semafaro", O_CREAT, 0666, 0);

    /* Processo 1 - Envio de mensagens */
    // Duplicação do processo atual
    pidProcesso1 = fork();

    if (pidProcesso1 == 0) {
        // Envio de cada mensagem
        for (int i = 0; i < QTD_MEN; i++) {
            sem_wait(semafaro); // Espera a leitura da mensagem
            sprintf(mensagem, "mensagem %d", i);
            printf("Processo 1: '%s'", mensagem);
            sem_post(semafaro); // Sinaliza o envio da mensagem
        }
        
    } else if (pidProcesso1 < 0) {
        printf("Erro ao criar Processo 1 para envio de mensagens.\n");
        exit(-1);
    }

    /* Processo 2 - Leitura de mensagens */
    // Duplicação do processo atual
    pidProcesso2 = fork();

    if (pidProcesso2 == 0) {
        for (int i = 1; i <= QTD_MEN; ++i) {
            sem_wait(semafaro); // Espera até que o Processo 1 envie a mensagem

            printf("Processo 2: '%s'\n", mensagem);

            sem_post(semafaro); // Sinaliza que a mensagem foi lida
        }
    } else if (pidProcesso2 < 0) {
        printf("Erro ao criar Processo 2 para leitura de mensagens.\n");
        exit(-1);
    }

    // Processo pai espera os processos filhos terminarem
    waitpid(pidProcesso1, NULL, 0);
    waitpid(pidProcesso2, NULL, 0);

    shmdt(mensagem);
    sem_close(semafaro);
    shmctl(segmento, IPC_RMID, NULL);

    return 0;
}