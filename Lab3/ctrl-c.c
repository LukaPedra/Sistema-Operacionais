#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#define EVER ;;
void intHandler(int sinal);
void quitHandler(int sinal);
void killhandler(int sinal);
int main(void)
{
	int a = getpid();
	printf("PID: %d\n", a);
	void (*p)(int); // ponteiro para função que recebe int como parâmetro
	p = signal(SIGINT, intHandler);
	printf("Endereco do manipulador anterior %p\n", p);
	p = signal(SIGQUIT, quitHandler);
	printf("Endereco do manipulador anterior %p\n", p);
	p = signal(SIGKILL, killhandler);
	printf("Endereco do manipulador anterior %p\n", p);

	puts("Ctrl-C desabilitado. Use Ctrl-\\ para terminar");
	for (EVER);
}
void intHandler(int sinal)
{
	printf("Você pressionou Ctrl-C (%d)\n", sinal);
}
void killhandler(int sinal)
{
	printf("Você matou o processo\n");
}
void quitHandler(int sinal)
{
	puts("Terminando o processo...");
	exit(0);
}