// Vetor de 10.000 posições começando com 5
// Criar 10 processos filhos
// Cada processo vai multiplicar por 2 e somar as posições
// Processo pai vai exibir o resultado

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>

#define TAM_MAX 10000
#define NUM_TRABALHADORES 10
#define PARCELA 1000

int main(int argc, char *argv[]) {
    int pid, status, segmento, somaTotal[NUM_TRABALHADORES];

    // Alocar memória compartilhada
    segmento = shmget (IPC_PRIVATE, 10*sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // Associar a memória compartilhada ao processo
    int a[TAM_MAX] = (int *) shmat(segmento, 0, 0);

    // Inicializar vetor com 5
    for (int i = 0; i < TAM_MAX; i++) {
        a[i] = 5;
    }

    // Criar processos filhos
    for (int i = 0; i < NUM_TRABALHADORES; i++) {
        pid = fork(); // Duplicar o processo para cada filho
        int soma = 0;

        if (pid < 0) {
            printf("Erro ao criar processo filho.\n");
            exit(-2);
        } 

        else if (pid == 0) {
            // Processo filho

            int comeco = i * PARCELA; // Começo da parcela no vetor
            int final = (i + 1) * PARCELA; // Final da parcela no vetor
            for(int p = comeco; p < final; p++) {
                a[p] *= 2;
                soma += a[p];
            }
            somaTotal[i] = soma; // Salvar resultado da soma

            exit(1);
        }
    }

    // Processo pai
    int somaParcelas = 0;
    for(int i = 0; i < NUM_TRABALHADORES; i++) {
        waitpid(-1, &status, 0);
        somaParcelas += somaTotal[i];
    }

    // Soma total das parcelas
    printf("Soma das parcelas: %d", somaParcelas);

    // libera a memória compartilhada do processo
    shmdt (a);
    // libera a memória compartilhada
    shmctl (segmento, IPC_RMID, 0);
    return 0;
}