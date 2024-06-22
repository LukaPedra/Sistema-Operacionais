#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

// Função para exibir o conteúdo de um arquivo
void printFile(char *file) {
	FILE *filePointer; 
  
	char *file2 = file; 
	char c;
  
	printf("\nConteúdo do arquivo lido:  \n"); 
  
	// Abrir o arquivo
	filePointer = fopen(file2, "r"); 
	if (filePointer == NULL) 
	{ 
		printf("Erro ao abrir o arquivo para leitura. \n"); 
		exit(0); 
	} 
  
	// Ler o conteúdo do arquivo
	c = fgetc(filePointer); 
	while (c != EOF) 
	{ 
		printf ("%c", c); 
		c = fgetc(filePointer); 
	} 
  
	fclose(filePointer);
}

// Função para criar diretórios e arquivos
void createDirectories() {
    mkdir("so", 0700);
    mkdir("so/a", 0700);
    mkdir("so/b", 0700);
    mkdir("so/c", 0700);
    
    FILE *fa = fopen("so/a/arqa.txt", "w");
    FILE *fb = fopen("so/b/arqb.txt", "w");
    FILE *fc = fopen("so/c/arqc.txt", "w");
    
    if (fa) {
        fprintf(fa, "Este é o arquivo arqa.txt\n");
        fclose(fa);
    }
    
    if (fb) {
        fprintf(fb, "Este é o arquivo arqb.txt\n");
        fclose(fb);
    }
    
    if (fc) {
        fprintf(fc, "Este é o arquivo arqc.txt\n");
        fclose(fc);
    }
}

// Função para exibir os atributos de um arquivo
void displayFileAttributes(const char *filepath) {
    struct stat st; // Estrutura para armazenar os atributos do arquivo

    // Obter os atributos do arquivo
    if (stat(filepath, &st) != 0) {
        perror("stat");
        return;
    }

    // Exibição dos atributos
    printf("Atributos do arquivo %s:\n", filepath);
    printf("Tamanho: %lld bytes\n", st.st_size);
    printf("Permissões: %o\n", st.st_mode & 0777);
    printf("Número de links: %hu\n", st.st_nlink);
    printf("ID do usuário (UID): %d\n", st.st_uid);
    printf("ID do grupo (GID): %d\n", st.st_gid);
    printf("Último acesso: %s", ctime(&st.st_atime));
    printf("Última modificação: %s", ctime(&st.st_mtime));
    printf("Última alteração dos atributos: %s", ctime(&st.st_ctime));
}

void modifyFileContent(const char *filepath, const char *conteudo, int offset) {
	FILE *filePointer; 

    // Abrir o arquivo
    filePointer = fopen(filepath, "r+"); 
    if (filePointer == NULL) 
    { 
        printf("Erro ao abrir o arquivo para escrita. \n"); 
        exit(0); 
    } 

    // Mover o ponteiro do arquivo para a posição desejada
    fseek(filePointer, offset, SEEK_SET);

    // Escrever no arquivo
    fprintf(filePointer, "%s", conteudo); // Novo conteúdo é escrito em cima dos antigos caraceteres

    fclose(filePointer);
}

// Função para listar recursivamente os sub-diretórios de um diretório
void listDirectories(const char *dirpath) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(dirpath)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
            printf("\n[%s]", entry->d_name);
            listDirectories(path);
        }
    }
    closedir(dir);
}