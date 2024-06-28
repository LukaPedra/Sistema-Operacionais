// Lucas Demarco Cambraia Lemos - 2110013
// Natalia Passini Grossmann - 2110456

#ifndef SIM_VIRTUAL_H
#define SIM_VIRTUAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct pagina {
  int modificada;    
  int usada;         
  int end;           
  int ultimo_acesso; 
};
typedef struct pagina Pagina;

struct tabela {
  Pagina **array_paginas;
  int quantidade;
  int capacidade;
};
typedef struct tabela Tabela;

Tabela *inicializa_tabela(int capacidade);
int set_endereco(int addr, int tamanho);
int verifica_endereco(Tabela *tabela, unsigned int addr, int tamanho, int tempo, char acao);
void add_pagina(Tabela *tabela, int addr, int tamanho, char *alg, int tempo);
void alg_LRU(Tabela *tabela, int addr, int tempo, int tamanho);
void alg_NRU(Tabela *tabela, int addr, int tempo, int tamanho);

#endif //SIM_VIRTUAL_H
