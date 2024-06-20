#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "aux.h"

int main(int argc, char *argv[]) {
    // Arquivos
	char *file1 = "so/a/arqa.txt";
	char *file2 = "so/b/arqb.txt";
    char *file3 = "so/c/arqc.txt";

    /* Item 1: Criação dos diretórios e dos arquivos */
	createDirectories();

	/* Item 2: Exibição dos atributos de um arquivo */
	displayFileAttributes(file1);

    /* Item 3: Leitura e exibição do conteúdo de um arquivo */
	printFile(file2);

    /* Item 4: Alteração do conteúdo de um arquivo */
    char *newContent = " There's no such thing as a painless lesson-they just don't exist. Sacrifices are necessary. You can't gain anything without losing something first. Although if you can endure that pain and walk away from it, you'll find that you now have a heart strong enough to overcome any obstacle. Yeah... a heart made Fullmetal.";
	modifyFileContent(file1, newContent, 10);
    
    // Leitura do arquivo
    printFile(file1);

    /* Item 5: Mudança de premissão de acesso à um arquivo */
    if (chmod(file1, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) != 0) {
        perror("chmod");
    }

    /* Item 6: */
	
    return 0;
}
