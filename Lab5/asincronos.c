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
    int shmid_buffer, shmid_produtor, shmid_consumidor, shmid_mensagens;
    char **mensagens;
    int *contador_produtor, *contador_consumidor;
    pid_t produtor, consumidor;
    sem_t *mutex, *vazio, *cheio;
    int status;

    // Create shared memory segments
    shmid_buffer = shmget(IPC_PRIVATE, sizeof(char *) * BUFFER_SIZE, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    shmid_produtor = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    shmid_consumidor = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    shmid_mensagens = shmget(IPC_PRIVATE, sizeof(char *) * BUFFER_SIZE * MAX_MSG_LEN, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    // Attach shared memory segments
    mensagens = (char **)shmat(shmid_buffer, 0, 0);
    contador_produtor = (int *)shmat(shmid_produtor, 0, 0);
    contador_consumidor = (int *)shmat(shmid_consumidor, 0, 0);

    // Create semaphores
    mutex = sem_open("mutex", O_CREAT, 0666, 1);
    vazio = sem_open("vazio", O_CREAT, 0666, BUFFER_SIZE);
    cheio = sem_open("cheio", O_CREAT, 0666, 0);

    // Initialize message buffer
    *contador_produtor = 0;
    *contador_consumidor = 0;
    char *buffer_mensagens = (char *)shmat(shmid_mensagens, 0, 0);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        mensagens[i] = buffer_mensagens + (i * MAX_MSG_LEN);
    }

    // Create processes
    produtor = fork();
    if (produtor < 0) {
        printf("Erro na criacao do processo produtor\n");
        exit(-1);
    } else if (produtor == 0) {
        for (int i = 0; i < 128; i++) {
            sem_wait(vazio);
            sem_wait(mutex);
            sprintf(mensagens[*contador_produtor], "mensagem %d", i + 1);
            printf("Produtor: %s\n", mensagens[*contador_produtor]);
            *contador_produtor = (*contador_produtor + 1) % BUFFER_SIZE;
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
        for (int i = 0; i < 128; i++) {
            sem_wait(cheio);
            sem_wait(mutex);
            printf("Consumido: %s\n", mensagens[*contador_consumidor]);
            *contador_consumidor = (*contador_consumidor + 1) % BUFFER_SIZE;
            sem_post(mutex);
            sem_post(vazio);
        }
        exit(0);
    }
    for (int i = 0; i < 2; i++) {
        wait(&status);
    }
    

    shmdt(mensagens);
    shmdt(contador_produtor);
    shmdt(contador_consumidor);
    shmdt(buffer_mensagens);
    shmctl(shmid_buffer, IPC_RMID, NULL);
    shmctl(shmid_produtor, IPC_RMID, NULL);
    shmctl(shmid_consumidor, IPC_RMID, NULL);
    shmctl(shmid_mensagens, IPC_RMID, NULL);

    sem_close(mutex);
    sem_close(vazio);
    sem_close(cheio);
    sem_unlink("mutex");
    sem_unlink("vazio");
    sem_unlink("cheio");

    return 0;
}
