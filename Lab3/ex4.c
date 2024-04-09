#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// Declaração da função
void errorHandler(int signum);

int main() {
    // Lidando o sinal SIGFPE
    signal(SIGFPE, errorHandler);

    // Operações com números reais
    float num1, num2;
    printf("Digite os números reais: ");
    scanf("%f %f", &num1, &num2);

    printf("Operações com números reais:\n");
    printf("Soma: %.2f\n", num1 + num2);
    printf("Subtração: %.2f\n", num1 - num2);
    printf("Multiplicação: %.2f\n", num1 * num2);
    printf("Divisão: %.2f\n", num1 / num2);

    // Operações com números inteiros
    int num3, num4;
    printf("\nDigite dois números inteiros: ");
    scanf("%d %d", &num3, &num4);

    printf("\nOperações com números inteiros:\n");
    printf("Soma: %d\n", num3 + num4);
    printf("Subtração: %d\n", num3 - num4);
    printf("Multiplicação: %d\n", num3 * num4);
    printf("Divisão: %d\n", num3 / num4);

    return 0;
}

// Função para lidar com o sinal SIGFPE
void errorHandler(int signal) {
    printf("Erro de divisão por zero.\n");
    exit(1);
}