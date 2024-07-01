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

// Variáveis globais
Process processes[NUM_PROCESSES];
Frame frames[NUM_FRAMES];
int current_time = 0;
int k;

// Função auxiliar para encontrar um quadro livre
int find_free_frame() {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (frames[i].process_id == -1) {
            return i;
        }
    }
    return -1;
}

// Função auxiliar para imprimir as tabelas de páginas
void print_page_tables() {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("Tabela de páginas para o processo P%d:\n", i+1);
        printf("Página\tQuadro\tVal.\tRef.\tMod.\n");
        for (int j = 0; j < NUM_PAGES; j++) {
            PageTableEntry page = processes[i].page_table[j];
            printf("%d\t%d\t%d\t%d\t%d\n", j, page.frame, page.valid, page.referenced, page.modified);
        }
        printf("\n");
    }
}

// Função auxiliar para o algoritmo de substituição NRU (Not Recently Used)
int subs_NRU(int process_id) {
    int class_0 = -1, class_1 = -1, class_2 = -1, class_3 = -1;
    
    for (int i = 0; i < NUM_PAGES; i++) {
        if (processes[process_id].page_table[i].valid) {
            int frame = processes[process_id].page_table[i].frame;
            bool r = processes[process_id].page_table[i].referenced;
            bool m = processes[process_id].page_table[i].modified;
            
            if (!r && !m) class_0 = i;
            else if (!r && m) class_1 = i;
            else if (r && !m) class_2 = i;
            else class_3 = i;
        }
    }
    
    int page_to_replace = (class_0 != -1) ? class_0 : 
                          (class_1 != -1) ? class_1 : 
                          (class_2 != -1) ? class_2 : class_3;
    
    // Substituir a página
    int frame = processes[process_id].page_table[page_to_replace].frame;
    processes[process_id].page_table[page_to_replace].valid = false;
    processes[process_id].page_table[page_to_replace].frame = -1;
    
    // Resetar os bits R para todas as páginas válidas
    for (int i = 0; i < NUM_PAGES; i++) {
        if (processes[process_id].page_table[i].valid) {
            processes[process_id].page_table[i].referenced = false;
        }
    }
    
    return frame;
}

// Função auxiliar para o algoritmo de substituição Segunda Chance
int subs_2ndCh(int process_id) {
    static int last_replaced[NUM_PROCESSES] = {0};
    int start = last_replaced[process_id];
    int checked_pages = 0;

    while (checked_pages < NUM_PAGES) {
        if (processes[process_id].page_table[start].valid) {
            if (processes[process_id].page_table[start].referenced) {
                processes[process_id].page_table[start].referenced = false;
            } else {
                int frame = processes[process_id].page_table[start].frame;
                processes[process_id].page_table[start].valid = false;
                processes[process_id].page_table[start].frame = -1;
                last_replaced[process_id] = (start + 1) % NUM_PAGES;
                return frame;
            }
        }
        start = (start + 1) % NUM_PAGES;
        checked_pages++;
    }
    
    return -1;
}


// Função auxiliar para o algoritmo de substituição LRU (Least Recently Used)
int subs_LRU(int process_id) {
    int lru_page = -1;
    int oldest_time = current_time;
    
    for (int i = 0; i < NUM_PAGES; i++) {
        if (processes[process_id].page_table[i].valid) {
            if (processes[process_id].page_table[i].last_used < oldest_time) {
                oldest_time = processes[process_id].page_table[i].last_used;
                lru_page = i;
            }
        }
    }
    
    int frame = processes[process_id].page_table[lru_page].frame;
    processes[process_id].page_table[lru_page].valid = false;
    processes[process_id].page_table[lru_page].frame = -1;
    
    return frame;
}

// Função auxiliar para o algoritmo de substituição Working Set
int subs_WS(int process_id) {
    int oldest_page = -1;
    int oldest_time = current_time;
    
    for (int i = 0; i < NUM_PAGES; i++) {
        if (processes[process_id].page_table[i].valid) {
            if (current_time - processes[process_id].page_table[i].last_used > k) {
                // Esta página não está no working set
                int frame = processes[process_id].page_table[i].frame;
                processes[process_id].page_table[i].valid = false;
                processes[process_id].page_table[i].frame = -1;
                return frame;
            }
            if (processes[process_id].page_table[i].last_used < oldest_time) {
                oldest_time = processes[process_id].page_table[i].last_used;
                oldest_page = i;
            }
        }
    }
    
    // Se todas as páginas estão no working set, remova a mais antiga
    if (oldest_page != -1) {
        int frame = processes[process_id].page_table[oldest_page].frame;
        processes[process_id].page_table[oldest_page].valid = false;
        processes[process_id].page_table[oldest_page].frame = -1;
        return frame;
    }
    
    // Não deveria chegar aqui, mas por segurança:
    return -1;
}

// Função para inicializar as estruturas
void initialize() {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        sprintf(processes[i].filename, "acessos_P%d", i+1);
        processes[i].access_file = fopen(processes[i].filename, "r");
        processes[i].page_faults = 0;
        
        for (int j = 0; j < NUM_PAGES; j++) {
            processes[i].page_table[j].valid = false;
            processes[i].page_table[j].frame = -1;
            processes[i].page_table[j].referenced = false;
            processes[i].page_table[j].modified = false;
            processes[i].page_table[j].last_used = 0;
            processes[i].page_table[j].age = 0;
        }
    }

    for (int i = 0; i < NUM_FRAMES; i++) {
        frames[i].process_id = -1;
        frames[i].page_number = -1;
    }
}

// Função para simular o acesso à memória
void access_memory(int process_id, int page_number, char access_type, int (*replace_func)(int)) {
    Process* process = &processes[process_id];
    PageTableEntry* page = &process->page_table[page_number];

    if (!page->valid) {
        // Page fault
        process->page_faults++;
        int empty_frame = -1;
        for (int i = 0; i < NUM_FRAMES; i++) {
            if (frames[i].process_id == -1) {
                empty_frame = i;
                break;
            }
        }

        if (empty_frame != -1) {
            // Alocar quadro vazio
            frames[empty_frame].process_id = process_id;
            frames[empty_frame].page_number = page_number;
            page->frame = empty_frame;
            page->valid = true;
        } else {
            // Substituir página
            int frame = replace_func(process_id);
        }
    }

    // Atualizar bits de acesso e modificação
    page->referenced = true;
    if (access_type == 'W') {
        page->modified = true;
    }

    // Atualizar informações para LRU e Aging
    page->last_used = current_time;
    page->age = (page->age >> 1) | 0x80000000;  // Shift right e set bit mais significativo

    current_time++;
}

// Função principal de simulação
void simulate(int num_rounds, int (*replace_func)(int)) {
    for (int round = 0; round < num_rounds; round++) {
        for (int i = 0; i < NUM_PROCESSES; i++) {
            int page_number;
            char access_type;
            if (fscanf(processes[i].access_file, "%d %c", &page_number, &access_type) == 2) {
                access_memory(i, page_number, access_type, replace_func);
            } else {
                // Reiniciar o arquivo se chegou ao fim
                rewind(processes[i].access_file);
                fscanf(processes[i].access_file, "%d %c", &page_number, &access_type);
                access_memory(i, page_number, access_type, replace_func);
            }
        }
    }

    // Imprimir tabelas de páginas
    print_page_tables();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Entrada inválida de parâmetros.\n");
        printf("Formato esperado: %s <algoritmo> <num_rodadas>\n", argv[0]);
        return 1;
    }

    char* algorithm = argv[1];
    int num_rounds = atoi(argv[2]);

    initialize();

    if (strcmp(algorithm, "NRU") == 0) {
        simulate(num_rounds, subs_NRU);
    } else if (strcmp(algorithm, "2ndChance") == 0) {
        simulate(num_rounds, subs_2ndCh);
    } else if (strcmp(algorithm, "LRU") == 0) {
        simulate(num_rounds, subs_LRU);
    } else if (strncmp(algorithm, "WS", 2) == 0) {
        int k = atoi(algorithm + 2);
        simulate(num_rounds, subs_WS); // Como mandar o k?
    } else {
        printf("Algoritmo desconhecido: %s\n", algorithm);
        printf("Possíveis algoritmos de subsituição:\n1- NRU\n2- 2ndChance\n3- LRU\n4- WS<k>\n");
        return 1;
    }

    // Imprimir resultados
    printf("Algoritmo de Substituição: %s\n", algorithm);
    printf("Rodadas executadas: %d\n", num_rounds);
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("Processo P%d - Page Faults: %d\n", i+1, processes[i].page_faults);
    }

    // Fechar arquivos e liberar recursos
    for (int i = 0; i < NUM_PROCESSES; i++) {
        fclose(processes[i].access_file);
    }

    return 0;
}