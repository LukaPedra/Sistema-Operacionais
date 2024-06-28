#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_PROCESSES 3
#define NUM_FRAMES 16
#define NUM_PAGES 16
#define MAX_FILENAME 20

// Estrutura para uma entrada na tabela de páginas
typedef struct {
    int frame;
    bool valid;
    bool referenced;
    bool modified;
    int last_used;  // Para LRU
    int age;        // Para Aging
} PageTableEntry;

// Estrutura para um processo
typedef struct {
    PageTableEntry page_table[NUM_PAGES];
    FILE* access_file;
    char filename[MAX_FILENAME];
    int page_faults;
} Process;

// Estrutura para um quadro de página
typedef struct {
    int process_id;
    int page_number;
} Frame;