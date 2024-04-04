
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <semaphore.h>

#define TAM_MAX 10000
#define NUM_TRABALHADORES 2

int main(int argc, char *argv[])
{
	int pid, status, segmento, *Vetor;

	// Alocar memória compartilhada
	segmento = shmget(IPC_PRIVATE, TAM_MAX * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	// Associar a memória compartilhada ao processo
	Vetor = (int *)shmat(segmento, 0, 0);

	// Inicializar vetor com 5
	for (int i = 0; i < TAM_MAX; i++)
	{
		Vetor[i] = 5;
	}

	// Criar processos filhos
	for (int i = 0; i < NUM_TRABALHADORES; i++)
	{
		pid = fork(); // Duplicar o processo para cada filho
		int soma = 0;

		if (pid < 0)
		{
			printf("Erro ao criar processo filho.\n");
			exit(-2);
		}

		else if (pid == 0)
		{
			// Processo filho

			for (int p = 0; p < TAM_MAX; p++)
			{
				Vetor[p] = Vetor[p] * 2 + 2;
			}

			exit(1);
		}
	}

	// Processo pai
	int somaParcelas = 0;
	for (int i = 0; i < NUM_TRABALHADORES; i++)
	{
		// waitpid(-1, &status, 0);
		wait(NULL);
	}
	for (int i = 0; i < TAM_MAX; i++)
	{
		if (Vetor[i] != 26)
		{
			printf("%d\n", Vetor[i]);
		}
	}

	// libera a memória compartilhada do processo
	shmdt(Vetor);
	// libera a memória compartilhada
	shmctl(segmento, IPC_RMID, 0);
	return 0;
}
