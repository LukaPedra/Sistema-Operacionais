#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

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
    mkdir("so", 0777);
    mkdir("so/a", 0777);
    mkdir("so/b", 0777);
    mkdir("so/c", 0777);
    
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

void displayFileAttributes
(const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) != 0) {
        perror("stat");
        return;
    }

    printf("Atributos do arquivo %s:\n", filepath);
    printf("Tamanho: %lld bytes\n", st.st_size);
    printf("Permissões: %o\n", st.st_mode & 0777);
    printf("Número de links: %hu\n", st.st_nlink);
    printf("ID do usuário (UID): %d\n", st.st_uid);
    printf("ID do grupo (GID): %d\n", st.st_gid);
    printf("Último acesso: %s", ctime(&st.st_atime));
    printf("Última modificação: %s", ctime(&st.st_mtime));
    printf("Última alteração: %s", ctime(&st.st_ctime));
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
void list_directories_recursively(const char *base_path, int indent) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(base_path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            list_directories_recursively(path, indent + 2);
        }
    }
    closedir(dir);
}