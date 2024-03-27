#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10

void read_command(char *command) {
    printf("$");
    fgets(command, MAX_COMMAND_LENGTH, stdin);

    // Remove o caractere de nova linha do final da entrada
    command[strcspn(command, "\n")] = '\0';
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_ARGUMENTS];
    int status;

    while (1) {
        read_command(command);

        // Dividir o comando em argumentos
        char *token = strtok(command, " ");
        int arg_count = 0;
        while (token != NULL && arg_count < MAX_ARGUMENTS - 1) {
            arguments[arg_count] = token;
            token = strtok(NULL, " ");
            arg_count++;
        }
        arguments[arg_count] = NULL; // Marcar o final da lista de argumentos

        // Executar o comando
        pid_t pid = fork();
		if (pid == 0) { // Processo filho
			if (execvp(arguments[0], arguments) == -1) {
				// Se o comando não foi encontrado, tente procurá-lo no diretório atual
				char cmd[MAX_COMMAND_LENGTH];
				sprintf(cmd, "./%s", arguments[0]);
				if (execvp(cmd, arguments) == -1) {
					perror("Erro ao executar o comando");
					exit(EXIT_FAILURE);
				}
			}
        } else if (pid < 0) { // Erro no fork
            perror("Erro ao criar um processo filho");
        } else { // Processo pai
            // Esperar pelo processo filho
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}
