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

#define MAX_MSG_LEN 15
#define BUFFER_SIZE 8

int main() {
    int shmid_buffer, shmid_contador, shmid_mensagens;
    char **mensagens;
    int *contador;
    pid_t produtor, consumidor;
    sem_t *mutex, *vazio, *cheio;
    int status;

    // Criar segmentos de memória compartilhada
    shmid_buffer = shmget(IPC_PRIVATE, sizeof(char *) * BUFFER_SIZE, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    shmid_contador = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    shmid_mensagens = shmget(IPC_PRIVATE, sizeof(char *) * BUFFER_SIZE * MAX_MSG_LEN, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    // Anexar segmentos de memória compartilhada
    mensagens = (char **)shmat(shmid_buffer, 0, 0);
    contador = (int *)shmat(shmid_contador, 0, 0);

    // Criar semáforos
    mutex = sem_open("mutex", O_CREAT, 0666, 1);
    vazio = sem_open("vazio", O_CREAT, 0666, BUFFER_SIZE);
    cheio = sem_open("cheio", O_CREAT, 0666, 0);

    // Inicializar buffer de mensagens
    *contador = 0;
    char *buffer_mensagens = (char *)shmat(shmid_mensagens, 0, 0);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        mensagens[i] = buffer_mensagens + (i * MAX_MSG_LEN);
    }

    // Criar processos
    produtor = fork();
    if (produtor < 0) {
        printf("Erro na criacao do processo produtor\n");
        exit(-1);
    } else if (produtor == 0) {
        // Processo produtor
        for (int i = 0; i < 128; i++) {
            sem_wait(vazio);
            sem_wait(mutex);
            sprintf(mensagens[*contador], "mensagem %d", i + 1);
            printf("Mensagem produzida: %s\n", mensagens[*contador]);
            *contador = (*contador + 1) % BUFFER_SIZE;
            sem_post(mutex);
            sem_post(cheio);
        }
        exit(0);
    }

    consumidor = fork();
    if (consumidor < 0) {
        printf("Erro na criacao do processo consumidor\n");
        exit(-1);
    } else if (consumidor == 0) {
        // Processo consumidor
        for (int i = 0; i < 128; i++) {
            sem_wait(cheio);
            sem_wait(mutex);
            printf("Mensagem consumida: %s\n", mensagens[*contador]);
            *contador = (*contador + 1) % BUFFER_SIZE;
            sem_post(mutex);
            sem_post(vazio);
        }
        exit(0);
    }

    // Esperar processos filho terminarem
    waitpid(produtor, &status, 0);
    waitpid(consumidor, &status, 0);

    // Desanexar e remover segmentos de memória compartilhada
    shmdt(mensagens);
    shmdt(contador);
    shmdt(buffer_mensagens);
    shmctl(shmid_buffer, IPC_RMID, NULL);
    shmctl(shmid_contador, IPC_RMID, NULL);
    shmctl(shmid_mensagens, IPC_RMID, NULL);

    // Fechar e remover semáforos
    sem_close(mutex);
    sem_close(vazio);
    sem_close(cheio);
    sem_unlink("mutex");
    sem_unlink("vazio");
    sem_unlink("cheio");

    return 0;
}