#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 10000
#define WORKERS 10

int main(void) {
	int a[SIZE];

    // Inicializar o vetor
    for (int i = 0; i < SIZE; i++) {
        a[i] = 5;
    }
    int segment_id, *sum;
	segment_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	sum = (int *)shmat(segment_id, NULL, 0);
	*sum = 0;
	for (int i = 0; i < WORKERS; i++) {
		pid_t pid = fork();
		if (pid == 0) {
			int start = i * (SIZE / WORKERS);
			int end = (i + 1) * (SIZE / WORKERS);
			for (int j = start; j < end; j++) {
				*sum += a[j] * 2 + j;
			}
			exit(0);
		} else if (pid < 0) {
			perror("Erro ao criar um processo filho");
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < WORKERS; i++) {
		wait(NULL);
	}

	printf("Soma: %d\n", *sum);
	shmdt(sum);
    shmctl(segment_id, IPC_RMID, NULL);

	return 0;
}