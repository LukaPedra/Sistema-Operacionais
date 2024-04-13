#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM_MAX 10000
#define NUM_THREADS 2

int vetor[TAM_MAX];

void *soma(void *arg);

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];

    // Inicializar vetor com 5
    for (int i = 0; i < TAM_MAX; i++) {
        vetor[i] = 5;
    }

    // Criar threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, soma, (void *) vetor) != 0) {
            printf("Erro ao criar a thread %d.\n", i);
            exit(-1);
        }
    }

    // Aguardar as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Erro ao aguardar a thread %d.\n", i);
            exit(-1);
        }
    }

    // Verificar valores do vetor
    for (int i = 0; i < TAM_MAX; i++) {
        if (vetor[i] != 26) {
            printf("%d\n", vetor[i]);
        }
    }

    return 0;
}

void *soma(void *arg) {
    int *vetor = (int *) arg;

    for (int p = 0; p < TAM_MAX; p++) {
        vetor[p] = vetor[p] * 2 + 2;
    }

    pthread_exit(NULL);
}