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
int alocateProcess(Queue *q, Process p);
char *concatenarStrings(const char *str1, const char *str2);
void execProcess(Process *p);
Process *achaProcessoRealTime(Queue *q, int sec);

// Globais:
int executing = FALSE;
int termina = FALSE;

int main(void)
{
	int shmid_process, shmid_pid;
	CurrentProcess *processInfo;
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

	// Quebra do loop infinito se receber interrupção do teclado (Ctrl+C)
	signal(SIGINT, handler);

	gettimeofday(&init, NULL);
	while (!termina)
	{
		gettimeofday(&end, NULL);
		sec = ((end.tv_sec - init.tv_sec) % 60);
		printf("\n%.1f'\n", sec);
		displayQueue(&filaPR);
		printf("Processo atual: %s\n", processInfo->p.name);

		// Recebe o processo do interpretador
		if (processInfo->escalonado == FALSE)
		{ 
			Queue *filaAux;

			// Alocação para fila de acordo com o seu tipo
			/* Fila REAL TIME */
			if (processInfo->p.policy == REAL_TIME)
			{
				filaAux = &filaRT;
			}

			/* Fila PRIORIDADE */
			else if (processInfo->p.policy == PRIORIDADE)
			{
				filaAux = &filaPR;
			}

			/* Fila ROUND ROBIN */
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

		// Exibição da fila de processos prontos
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
			// Espera do ROUND ROBIN
			else if (p.policy == ROUND_ROBIN)
			{
				kill(p.pid, SIGSTOP);
				dequeue(&filaRR);

				enqueue(&filaProntos, p);
				
				executing = FALSE;
			}
		}
		
		// Inicia ou continua a execução dos processos
		if (executing == FALSE)
		{
			/* Execução do REAL TIME */
			//Encontra se existe um processo para executar neste segundo
			Process* aux = achaProcessoRealTime(&filaRT, sec);
			if (aux != NULL)
			{ // Primeiro da fila entra em execução
				secIni = sec;
				p = *aux;

				if (!p.started)
				{
					execProcess(&p); // Executa processo pela primeira vez
					p.started = TRUE; // diz que o processo começou
				}
				else
				{
					printf("Continuando o processo %s %d\n", p.name,p.pid);
					kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
				}

				executing = TRUE;
			}

			/* Execução do PRIORIDADE */
			else if (!isEmpty(&filaPR))
			{
				p = filaPR.front->process;
				if (!p.started)
				{
					execProcess(&p); // Executa processo pela primeira vez
					p.started = TRUE; // diz que o processo começou
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
					p.started = TRUE; // diz que o processo começou
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
	// Libera a memória compartilhada
	shmctl(shmid_process, IPC_RMID, 0);
	shmctl(shmid_pid, IPC_RMID, 0);

	return 0;
}

int alocateProcess(Queue *q, Process p)
{
	enqueue(q, p);
	// Organização da fila de acordo com o tipo do processo
	if (q->Type != ROUND_ROBIN)
	{
		queueSort(q); // Prioridade e Real Time precisam ser organizado as filas
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
	/* Processo Filho */
    if (aux == 0) {
        execvp(path, argv);
        perror("execvp failed");
        exit(1);
    } 
	
	/* Processo Pai */
	else if (aux > 0) {
        p->pid = aux;
    } 
	
	/* Falha na criação do processo */
	else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    return;
}
