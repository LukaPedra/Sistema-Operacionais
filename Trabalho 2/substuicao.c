#include "substituicao.h"

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