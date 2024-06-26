#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

// Protótipos de funções
void printFile(char *file);
void createDirectories();
void displayFileAttributes(const char *filepath);
void modifyFileContent(const char *filepath, const char *conteudo, int offset);
void listDirectories(const char *dirpath);
