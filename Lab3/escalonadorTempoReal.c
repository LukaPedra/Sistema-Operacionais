#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0

int main() {
    struct timeval start_time; // Obter o tempo inicial
    struct timeval current_time; // Obter o tempo atual

    long int sec_inicio;
    long int sec_atual;
    long int dif;

	int p1_state = FALSE;
	int p2_state = FALSE;
	int p3_state = FALSE;

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
				printf("P1 rodando @ %ld\n", dif);
				if (p3_state == TRUE) {
					printf("P3 parado @ %ld\n", dif);
					kill(p3, SIGSTOP);
					p3_state = FALSE;
				}
				kill(p1, SIGCONT);
				p1_state = TRUE;
			} 
			if((dif % 60) == (p1_duration + p1_start)) {
				printf("P1 parado @ %ld\n", dif);
				kill(p1, SIGSTOP);
				p1_state = FALSE;
			}
			if((dif % 60) == p2_start) {
				printf("P2 rodando @ %ld\n", dif);
				if (p3_state == TRUE) {
					printf("P3 parado @ %ld\n", dif);
					kill(p3, SIGSTOP);
					p3_state = FALSE;
				}
				kill(p2, SIGCONT);
				p2_state = TRUE;
			}
			if((dif % 60) == 0) {
				printf("P2 parado @ %ld\n", dif);
				kill(p2, SIGSTOP);
				p2_state = FALSE;
			}
			if(p1_state == FALSE && p2_state == FALSE && p3_state == FALSE){
				printf("P3 rodando @ %ld\n", dif);
				kill(p3, SIGCONT);
				p3_state = TRUE;
			}
			usleep(500000); // 500ms
        }
    }

    // Processo filho 1
    if (p1 == 0) {
		while(1){
			sleep(5);
        }
    }

    // Processo filho 2
    if (p2 == 0) {
        while(1){

			sleep(5);
        }
    }

    // Processo filho 3
    if (p3 == 0) {
        while(1){
			sleep(5);
        }
    }

    return 0;
}
