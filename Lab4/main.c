#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <pthread.h>

#define TAM_MAX 10000
#define NUM_THREADS 10
#define PARCELA 10


typedef struct {
    int *a;
    int start;
    int end;
} thread_args;

int *soma(int *a, int inicio, int fim) {
    int s = 0;
    for (int i = inicio; i < fim; i++) {
        s += a[i];
    }
    return s;
}

int main(int argc, char *argv[]) {
    int a[TAM_MAX];
	pthread_t threads[NUM_THREADS];
	thread_args *args = malloc(sizeof(thread_args));
	args->a = a;
	args->start = t*PARCELA;
	args->end = (t+1)*PARCELA;

    // Inicializar vetor com 5
    for (int i = 0; i < TAM_MAX; i++) {
        a[i] = 5;
    }
	for(int t=0;t < NUM_THREADS;t++)
	{
		printf("Creating thread %d\n", t);
		pthread_create(&threads[t], NULL, soma, a, t*PARCELA, (t+1)*PARCELA);
	}
}