#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>

int main() {
    struct timeval start_time; // Obter o tempo inicial
    struct timeval current_time; // Obter o tempo atual

    long int sec_inicio;
    long int sec_atual;
    long int dif;

    // Definição dos tempos de início e duração de cada processo
    int p1_start = 5;
    int p1_duration = 20;
    int p2_start = 45;
    int p2_duration = 15;

    // Duplicação do processo atual para os processos filhos
    pid_t p1, p2, p3;
    p1 = fork();
    p2 = fork();
    p3 = fork();

    // Processo pai
    if (p1 > 0 && p2 > 0 && p3 > 0 ){
        // Obter o tempo inicial
        gettimeofday(&start_time, NULL);
        sec_inicio = start_time.tv_sec;

        // Loop infinito
        while (1) {
            // Obter o tempo atual
            gettimeofday(&current_time, NULL);
            sec_atual = current_time.tv_sec;
            dif = sec_atual - sec_inicio;

            if((dif % 60) == p1_start) {
                kill(p3, SIGSTOP);
                kill(p1, SIGCONT);
            } 
            if((dif % 60) == (p1_duration + p1_start)) {
                kill(p1, SIGSTOP);
                kill(p3, SIGCONT);
            }
            if((dif % 60) == p2_start) {
                kill(p3, SIGSTOP);
                kill(p2, SIGCONT);
            }
            if((dif % 60) == 0) {
                kill(p2, SIGSTOP);
                kill(p3, SIGCONT);
            }
        }
    }

    // Processo filho 1
    if (p1 == 0) {
        kill(getpid(), SIGSTOP);
        printf("Iniciando processo 1.\n");
        return 0;
    }

    // Processo filho 2
    if (p2 == 0) {
        kill(getpid(), SIGSTOP);
        printf("Iniciando processo 2.\n");
        return 0;
    }

    // Processo filho 3
    if (p3 == 0) {
        kill(getpid(), SIGSTOP);
        printf("Iniciando processo 3.\n");
        return 0;
    }

    return 0;
}
