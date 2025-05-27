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
void execProcess(Process *p);
Process *achaProcessoRealTime(Queue *q, int sec);

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
	if (shmid_process == -1)
	{
		perror("Erro ao criar a memória compartilhada do shmid_process.\n");
		exit(1);
	}
	processInfo = (CurrentProcess *)shmat(shmid_process, 0, 0);

	Queue filaRR;
	Queue filaRT;
	Queue filaPR;
	Queue filaProntos;
	initQueue(&filaRR, ROUND_ROBIN);
	initQueue(&filaRT, REAL_TIME);
	initQueue(&filaPR, PRIORIDADE);
	initQueue(&filaProntos, PRONTOS);

	Process p;

	signal(SIGINT, handler); // Se receber interrupção do teclado (Ctrl+C), QUEBRA O LOOP INFINITO

	gettimeofday(&init, NULL);
	while (!termina)
	{

		gettimeofday(&end, NULL);
		sec = ((end.tv_sec - init.tv_sec) % 60);
		printf("\n%.1f'\n", sec);
		displayQueue(&filaPR);
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
			if (processInfo->escalonado == TRUE)
			{
				printf("Processo %s escalonado\n", processInfo->p.name);
			}
		}
		//displayQueue(&filaRT);
		//displayQueue(&filaPR);
		// displayQueue(&filaRR);
		displayQueue(&filaProntos);

		//Tratamento de processos em execução
		if (executing == TRUE)
		{
			if (p.policy == REAL_TIME)
			{
				if (sec == secIni + p.duration)
				{
					printf("Processo %s terminou\n", p.name);
					kill(p.pid, SIGSTOP);

					// dequeue(&filaRT);
					// enqueue(&filaRT, p);
					// printf("\n\nFila Real Time:\n");
					// displayQueue(&filaRT); //Imprime Fila de processos Real Time
					executing = FALSE;
				}
			}
			else if ((p.policy == PRIORIDADE)&&(sec == p.init + 5.0))
			{
				kill(p.pid, SIGSTOP);
				dequeue(&filaPR);
				displayQueue(&filaPR);
				enqueue(&filaProntos, p);
				executing = FALSE;
			}
			/* Espera do ROUND ROBIN */
			else if (p.policy == ROUND_ROBIN)
			{
				kill(p.pid, SIGSTOP);
				dequeue(&filaRR);

				enqueue(&filaProntos, p);
				
				executing = FALSE;
			}
		}
		
		/*Inicia ou continua a execução dos processos*/
		if (executing == FALSE)
		{
	
			/* Execução do REAL TIME */
			// if ((!isEmpty(&filaRT)) && (sec == filaRT.front->process.init))
			Process* aux = achaProcessoRealTime(&filaRT, sec);//Encontra se existe um processo para executar neste segundo
			if (aux != NULL)
			{ // Primeiro da fila entra em execução
				secIni = sec;
				p = *aux;

				if (!p.started)
				{
					execProcess(&p); // Executa processo pela primeira vez
					
					p.started = TRUE;		// diz que o processo começou
				}
				else
				{
					printf("Continuando o processo %s %d\n", p.name,p.pid);
					kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
				}

				executing = TRUE;
			}
			//Execução do PRIORIDADE
			else if (!isEmpty(&filaPR))
			{
				p = filaPR.front->process;
				if (!p.started)
				{
					execProcess(&p);	// Executa processo pela primeira vez
					p.started = TRUE;	// diz que o processo começou
					p.init = sec;

				}
				else
				{
					printf("Continuando o processo %s %d\n", p.name,p.pid);
					kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
				}

				executing = TRUE;
			}

			/* Execução do ROUND ROBIN */
			else if (!isEmpty(&filaRR))
			{
				p = filaRR.front->process;
				if(!p.started){
					execProcess(&p); // Executa processo pela primeira vez
					// p.pid = *pid;			// pega o PID do processo
					p.started = TRUE;		// diz que o processo começou
				}
				else{
					printf("Continuando o processo %s %d\n", p.name,p.pid);

					kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
				}
				executing = TRUE;
			}
		}
		if (executing == TRUE){
			printf("Processo %s em execução\n", p.name);
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
	enqueue(q, p);
	// após botar na fila ele organiza a fila de acordo com o tipo do processo
	if (q->Type != ROUND_ROBIN)
	{
		queueSort(q); //Prioridade e Real Time precisam ser organizado as filas
	}
	return TRUE;
}

void handler(int sig)
{
	termina = TRUE;
}


Process *achaProcessoRealTime(Queue *q, int sec)
{
	if (isEmpty(q))
	{
		return NULL;
	}
	Node *temp = q->front;
	while (temp != NULL)
	{
		if (temp->process.init == sec)
		{
			return &temp->process;
		}
		temp = temp->next;
	}
	return NULL;
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

void execProcess(Process *p)
{
    char inicioPath[] = "./Processos/";
    char *path;
    pid_t aux;
    printf("%s Iniciando pela primeira vez\n", p->name);

    path = concatenarStrings(inicioPath, p->name);

    char *argv[] = {path, NULL};

    aux = fork();
    if (aux == 0) {
        // This is the child process.
        execvp(path, argv);
        perror("execvp failed"); // execvp() only returns if an error occurred
        exit(1);
    } else if (aux > 0) {
        // This is the parent process.
        p->pid = aux;
    } else {
        // Fork failed.
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    return;
}
