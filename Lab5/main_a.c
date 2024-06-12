// Lucas Demarco Cambraia Lemos - 2110013
// Natalia Passini Grossmann - 2110456

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

int main() {
  int segment1;
  char *message;
  pid_t p1, p2;
  sem_t *semaphore;
  int status;

  segment1 = shmget(IPC_PRIVATE, sizeof(char) * MAX_MSG_LEN, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  message = (char *)shmat(segment1, 0, 0);
  semaphore = sem_open("semaphore", O_CREAT, 0666, 0);
  sem_init(semaphore, 1, 1);

  p1 = fork();
  if (p1 < 0) {
    printf("Erro na criacao do processo 1\n");
    exit(-1);
  }
  if (p1 == 0) {
    // PRODUTOR
    char aux[MAX_MSG_LEN];
    for(int i = 0; i < 128; i++) {
      sem_wait(semaphore);
      sprintf(aux, "mensagem %d", i + 1);
      strcpy(message, aux);
      printf("Mensagem produzida: %s\n", message);
      sem_post(semaphore);
      sleep(1);
    }
  }

  p2 = fork();
  if (p2 < 0) {
    printf("Erro na criacao do processo 2\n");
    exit(-1);
  }
  if (p2 == 0) {
    int num_msg = 0;
    // CONSUMIDOR
    while(1) {
      sem_wait(semaphore);
      printf("Mensagem consumida: %s\n", message);
      if(num_msg == 128) {
        break;
      } else {
        num_msg++;
      }
      sem_post(semaphore);
      sleep(1);
    }
  }

  for(int i = 0; i < 2; i++) {
    wait(&status);
  }

  shmdt(message);
  sem_destroy(semaphore);
  shmctl(segment1, IPC_RMID, NULL);

  return 0;
}
