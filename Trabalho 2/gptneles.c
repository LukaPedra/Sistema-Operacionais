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


// Função auxiliar para encontrar um quadro livre
int find_free_frame() {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (frames[i].process_id == -1) {
            return i;
        }
    }
    return -1;
}

// Função NRU (Not Recently Used)
void nru_replace(int process_id) {
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

// Função Segunda Chance
void second_chance_replace(int process_id) {
    static int last_replaced[NUM_PROCESSES] = {0};
    int start = last_replaced[process_id];
    
    while (1) {
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
    }
}

// Função LRU (Least Recently Used)
void lru_replace(int process_id) {
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

// Função Working Set
void working_set_replace(int process_id, int k) {
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
void access_memory(int process_id, int page_number, char access_type) {
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
            // Aqui você chamaria a função de substituição de página adequada
            // Por exemplo: nru_replace(process_id);
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
void simulate(int num_rounds, void (*replace_func)(int)) {
    for (int round = 0; round < num_rounds; round++) {
        for (int i = 0; i < NUM_PROCESSES; i++) {
            int page_number;
            char access_type;
            if (fscanf(processes[i].access_file, "%d %c", &page_number, &access_type) == 2) {
                access_memory(i, page_number, access_type);
            } else {
                // Reiniciar o arquivo se chegou ao fim
                rewind(processes[i].access_file);
                fscanf(processes[i].access_file, "%d %c", &page_number, &access_type);
                access_memory(i, page_number, access_type);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <algoritmo> <num_rodadas>\n", argv[0]);
        return 1;
    }

    char* algorithm = argv[1];
    int num_rounds = atoi(argv[2]);

    initialize();

    if (strcmp(algorithm, "NRU") == 0) {
        simulate(num_rounds, nru_replace);
    } else if (strcmp(algorithm, "2ndChance") == 0) {
        simulate(num_rounds, second_chance_replace);
    } else if (strcmp(algorithm, "LRU") == 0) {
        simulate(num_rounds, lru_replace);
    } else if (strncmp(algorithm, "WS", 2) == 0) {
        int k = atoi(algorithm + 2);
        // Implementar chamada para Working Set
    } else {
        printf("Algoritmo desconhecido: %s\n", algorithm);
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