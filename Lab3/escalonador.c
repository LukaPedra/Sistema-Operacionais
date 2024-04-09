#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int child1_pid, child2_pid;


void escalonar(int estado) {
	if (estado % 2 == 0) {
		printf("Escalonando processo 1\n");
		// Parar o processo filho 1
		kill(child1_pid, SIGSTOP);
		// Continuar o processo filho 2
		kill(child2_pid, SIGCONT);
	} else {
		printf("Escalonando processo 2\n");
		// Parar o processo filho 2
		kill(child2_pid, SIGSTOP);
		// Continuar o processo filho 1
		kill(child1_pid, SIGCONT);
	}
	
}

int main(int argc, char *argv[]) {
    // Criar processo filho 1
    child1_pid = fork();
    if (child1_pid == 0) {
        // Código do processo filho 1
		//I am trying to execute the command passed as argument
		execvp(argv[1], argv);
		kill(getpid(), SIGSTOP);
        return 0;
    }

    // Criar processo filho 2
    child2_pid = fork();
    if (child2_pid == 0) {
        // Código do processo filho 2
        execvp(argv[1], argv);
		kill(getpid(), SIGSTOP);
        return 0;
    }

    

	for (int i = 0; i < 15; i++) {
		printf("Tempo: %d\n", i);
		// Executar processo filho 1 por 1 segundo
		escalonar(i);
		sleep(1);
	}
    // Matar os processos filhos
    kill(child1_pid, SIGKILL);
    kill(child2_pid, SIGKILL);

    return 0;
}