#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM_MAX 100000
#define NUM_THREADS 100
#define FALSE 1
#define TRUE 0

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
    int verificador = -1;
    for (int i = 0; i < TAM_MAX - 1; i++) {
        if (vetor[i] != vetor[i + 1]) {
            verificador = FALSE;
        } else {
            verificador = TRUE;
        }
    }

    // Exibir resultado
    if (verificador == FALSE) {
            printf("Elementos do vetor possuem valores distintos.\n");
        } else {
            printf("Elementos do vetor possuem o mesmo valor.\n");
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