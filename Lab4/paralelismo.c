#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TAM_MAX 10000
#define NUM_THREADS 10
#define PARCELA 1000

int a[TAM_MAX];
int parcelas[NUM_THREADS];

void *soma(void *arg);

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Inicializar vetor com 5
    for (int i = 0; i < TAM_MAX; i++) {
        a[i] = 5;
    }

    // Criar threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, soma, &ids[i]) != 0) {
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

    // Soma total das parcelas
    int somaParcelas = 0;
    for(int i = 0; i < NUM_THREADS; i++) {
        somaParcelas += parcelas[i];
    }
    printf("Soma das parcelas: %d\n", somaParcelas);

    return 0;
}

void *soma(void *arg) {
    int id = *((int *) arg);
    int start = id * PARCELA; // Começo da parcela no vetor
    int end = (id + 1) * PARCELA; // Final da parcela no vetor
    int soma = 0;

    for(int p = start; p < end; p++) {
        soma += a[p] * 2;
    }

    parcelas[id] = soma;

    pthread_exit(NULL);
}