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

#define MAX_TAM_MEN 15
#define QTD_MEN 128

int main(int argc, char *argv[]) {
    int segmento;
    pid_t pidProcesso1, pidProcesso2;
    char* mensagem;
    sem_t* semafaro;
    int status;

    // Alocar memória compartilhada
    segmento = shmget(IPC_PRIVATE, MAX_TAM_MEN * sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


    // Associar a memória compartilhada ao processo
    mensagem = (char*)shmat(segmento, 0, 0);
    semafaro = sem_open("semafaro", O_CREAT, 0666, 0);
    sem_init(semafaro, 1, 1);

    pidProcesso1 = fork();

    if (pidProcesso1 == 0) {
        char aux[MAX_TAM_MEN];
        // Envio de cada mensagem
        for (int i = 1; i <= QTD_MEN; i++) {
            sem_wait(semafaro); // Espera a leitura da mensagem
            sprintf(aux, "mensagem %d", i);
            strcpy(mensagem, aux);
            printf("Processo 1: '%s'\n", mensagem);
            sem_post(semafaro); // Sinaliza o envio da mensagem
            sleep(1);
        }
        
        exit(0);

    } else if (pidProcesso1 < 0) {
        perror("Erro ao criar Processo 1 para envio de mensagens");
        exit(-1);
    }

    /* Processo 2 - Leitura de mensagens */
    // Duplicação do processo atual
    pidProcesso2 = fork();

    if (pidProcesso2 == 0) {
        for (int i = 1; i <= QTD_MEN; i++) {
            sem_wait(semafaro); // Espera até que o Processo 1 envie a mensagem
            printf("Processo 2: '%s'\n", mensagem);
            sem_post(semafaro); // Sinaliza que a mensagem foi lida
            sleep(1);
        }

        exit(0);

    } else if (pidProcesso2 < 0) {
        perror("Erro ao criar Processo 2 para leitura de mensagens");
        exit(-1);
    }

    for (int i = 0; i < 2; i++) {
        wait(&status);
    }

    /* Fechar semáforo e liberar memória compartilhada */
    shmdt(mensagem);
    sem_destroy(semafaro);
    shmctl(segmento, IPC_RMID, NULL);

    return 0;
}