// Lucas Demarco Cambraia Lemos - 2110013
// Natalia Passini Grossmann - 2110456

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(void) {
    struct timeval hora_inicio;
    struct timeval hora_atual;
    long int sec_inicio;
    long int sec_atual;
    long int dif;
    pid_t p1, p2, p3;

    p1 = fork();
    p2 = fork();
    p3 = fork();

    if(p1 > 0 && p2 > 0 && p3 > 0) {
        gettimeofday(&hora_inicio, NULL);
        sec_inicio = hora_inicio.tv_sec;

        while(1) {
            gettimeofday(&hora_atual, NULL);
            sec_atual = hora_atual.tv_sec;
            dif = sec_atual - sec_inicio;

            if((dif % 60) == 5) {
                kill(p3, SIGSTOP);
                kill(p1, SIGCONT);
            } 
            if((dif % 60) == 25) {
                kill(p1, SIGSTOP);
                kill(p3, SIGCONT);
            }
            if((dif % 60) == 45) {
                kill(p3, SIGSTOP);
                kill(p2, SIGCONT);
            }
            if((dif % 60) == 0) {
                kill(p2, SIGSTOP);
                kill(p3, SIGCONT);
            }
        }
    }

    if(p1 == 0) {
        kill(getpid(), SIGSTOP);
        while(1){
            printf("processo1\n");
        }
    }
    if(p2 == 0) {
        kill(getpid(), SIGSTOP);
        while(1){
            printf("processo2\n");
        }
    }
    if(p3 == 0) {
        kill(getpid(), SIGSTOP);
        while(1){
            printf("processo3\n");
        }
    }

    return 0;
}