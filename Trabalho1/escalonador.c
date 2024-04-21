#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

#include "info.h"

// Protótipos:
void handler(int sig);
// void handler1(int sig);
int alocateProcess(Queue *q, Process p);
char *concatenarStrings(const char *str1, const char *str2);
void execProcess(Process p, pid_t *pid);

// Globais:
//  int io_bound = FALSE;
int executing = FALSE;
int termina = FALSE;

int main(void)
{
	int shmid_process, shmid_pid;
	CurrentProcess *processInfo;
	// pid_t *pid = malloc(sizeof(pid_t));
	// if (pid == NULL) {
	// 	perror("Failed to allocate memory for pid");
	// 	exit(EXIT_FAILURE);
	// }
	pid_t *pid;

	struct timeval init, end;
	float sec;
	float secIni;

	// Anexar à memória compartilhada do processo recebido do interpretador
	shmid_pid = shmget(SHM_KEY2, sizeof(pid_t), IPC_CREAT | 0666);
	if (!shmid_pid){ perror("Erro ao criar a memória compartilhada do shmid_pid.\n"); exit(1);}
	pid = shmat(shmid_pid, 0, 0);

	// Anexar à memória compartilhada do pid recebido pelo processo executado na fila
	shmid_process = shmget(SHM_KEY, sizeof(CurrentProcess), IPC_CREAT | 0666);
	if (!shmid_process)
	{
		perror("Erro ao criar a memória compartilhada do shmid_process.\n");
		exit(1);
	}
	processInfo = (CurrentProcess *)shmat(shmid_process, 0, 0);

	Queue filaRR;
	Queue filaRT;
	Queue filaPR;
	initQueue(&filaRR, ROUND_ROBIN);
	initQueue(&filaRT, REAL_TIME);
	initQueue(&filaPR, PRIORIDADE);

	Process p;

	signal(SIGINT, handler); // Se receber interrupção do teclado (Ctrl+C), QUEBRA O LOOP INFINITO

	gettimeofday(&init, NULL);
	while (!termina)
	{
		// printa a fila rr

		gettimeofday(&end, NULL);
		sec = ((end.tv_sec - init.tv_sec) % 60);
		printf("\n%.1f'\n", sec);
		printf("Processo atual: %s\n", processInfo->p.name);
		// Receve o processo do interpretador
		if (processInfo->escalonado == FALSE)
		{ /*Se ainda recebe processo entra aqui*/
			Queue *filaAux;

			if (processInfo->p.policy == REAL_TIME)
			{
				filaAux = &filaRT;
			}
			else if (processInfo->p.policy == PRIORIDADE)
			{
				filaAux = &filaPR;
			}
			else if (processInfo->p.policy == ROUND_ROBIN)
			{
				filaAux = &filaRR;
			}

			processInfo->escalonado = alocateProcess(filaAux, processInfo->p);
		}
		displayQueue(&filaRT);
		displayQueue(&filaPR);
		displayQueue(&filaRR);

			/*Inicia a execução dos processos*/
			if (executing == FALSE)
			{

				/* Execução do REAL TIME */
				if ((!isEmpty(&filaRT)) && (sec == filaRT.front->process.init))
				{ // Primeiro da fila entra em execução
					secIni = sec;
					p = filaRT.front->process;

					if (!p.started)
					{
						printf("pid dentro do struct = %d\n", p.pid);
						execProcess(p, &p.pid); // Executa processo pela primeira vez
						printf("pid fora do struct = %d\n", *pid);
						printf("Processo %s iniciado com PID %d\n", p.name, p.pid);
						
						// p.pid = *pid;			// pega o PID do processo
						p.started = TRUE;		// diz que o processo começou
					}
					else
					{
						kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
					}

					executing = TRUE;
				}
				else if (!isEmpty(&filaPR))
				{
					p = filaPR.front->process;
					if (!p.started)
					{
						execProcess(p, &p.pid); // Executa processo pela primeira vez
						// p.pid = *pid;			// pega o PID do processo
						p.started = TRUE;		// diz que o processo começou
					}
					else
					{
						kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
					}

					executing = TRUE;
				}

				/* Execução do ROUND ROBIN */
				else if (!isEmpty(&filaRR))
				{
					printf("Entrou no Round Robin\n");
					printf("Processo começou: %d\n", filaRR.front->process.started);
					p = filaRR.front->process;
					if(!p.started){
						execProcess(p, pid); // Executa processo pela primeira vez
						// p.pid = *pid;			// pega o PID do processo
						p.started = TRUE;		// diz que o processo começou
					}
					else{
						kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
					}
					executing = TRUE;
				}
			}
			if (executing == TRUE)
			{
				// p.pid = *pid;

				if (p.policy == REAL_TIME)
				{
					if (sec == secIni + p.duration)
					{
						printf("Processo %s terminou\n", p.name);
						kill(p.pid, SIGSTOP);

						dequeue(&filaRT);
						enqueue(&filaRT, p);
						// printf("\n\nFila Real Time:\n");
						// displayQueue(&filaRT); //Imprime Fila de processos Real Time
						executing = FALSE;
					}
				}
				else if (p.policy == PRIORIDADE)
				{
					kill(p.pid, SIGSTOP);
					dequeue(&filaPR);
					enqueue(&filaPR, p);
					// printf("\n\nFila Prioridade:\n");
					// displayQueue(&filaPR); //Imprime Fila de processos Prioridade
					executing = FALSE;
				}
				/* Espera do ROUND ROBIN */
				else
				{
					kill(p.pid, SIGSTOP);
					dequeue(&filaRR);

					enqueue(&filaRR, p);
					// printf("\n\nFila Round Robin:\n");
					// displayQueue(&filaRR); //Imprime Fila de processos Round Robin
					executing = FALSE;
				}
			}
		sleep(1);
	}
	free(pid);
	/* Libera a memória compartilhada */
	shmctl(shmid_process, IPC_RMID, 0);
	shmctl(shmid_pid, IPC_RMID, 0);

	return 0;
}

int alocateProcess(Queue *q, Process p)
{
	if (p.policy == PRIORIDADE)
	{
		// organizar a fila de prioridade por prioridade
	}
	enqueue(q, p);
	// após botar na fila ele organiza a fila de acordo com o tipo do processo
	if (q->Type != ROUND_ROBIN)
	{
		queueSort(q);
	}

	return TRUE;
}

void handler(int sig)
{
	termina = TRUE;
}

char *concatenarStrings(const char *str1, const char *str2)
{
	size_t tamanhoStr1 = strlen(str1);
	size_t tamanhoStr2 = strlen(str2);
	size_t tamanhoTotal = tamanhoStr1 + tamanhoStr2 + 1;

	char *resultado = (char *)malloc(tamanhoTotal);

	if (resultado == NULL)
	{
		perror("Erro ao alocar memória");
		exit(1);
	}

	strcpy(resultado, str1);
	strcat(resultado, str2);

	return resultado;
}

void execProcess(Process p, pid_t *pid)
{
	char inicioPath[] = "./Processos/";
	char *path;
	pid_t aux;
	printf("%s rodando\n", p.name);

	path = concatenarStrings(inicioPath, p.name);

	char *argv[] = {path, NULL};

	// aux = fork();
	// if (aux == 0){
	// 	*pid = getpid();
	// }
	// if (aux == 0)
	// {
	// 	printf("Iniciando o programa %s\n", path);
	// 	execvp(path, argv);
	// 	perror("execvp failed"); // execvp() only returns if an error occurred
	// 	exit(1);
	// }
	aux = fork();
	if (aux == 0){
		// This is the child process.
		printf("Iniciando o programa %s\n", path);
		execvp(path, argv);
		perror("execvp failed"); // execvp() only returns if an error occurred
		exit(1);
	} else if (aux > 0) {
		// This is the parent process.
		*pid = aux;
		p.pid = *pid;
	} else {
		// Fork failed.
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	return;
}
