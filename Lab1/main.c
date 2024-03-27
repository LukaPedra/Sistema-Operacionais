#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	int n = 3;
	pid_t pid = fork();
	int status;
	if (pid != 0)
	{
		for (int i = 0; i < 10000; i++)
		{
			n++;
		}
		printf("Processo pai, pid=%d, n=%d\n", getpid(), n);
		waitpid(-1, &status, 0);
		printf("%d",status);
	}
	else
	{
		for (int i = 0; i < 10000; i++)
		{
			n = n + 10;
		}
		printf("Processo filho, pid=%d, n=%d", getpid(), n);
		exit(0);
	}
}
