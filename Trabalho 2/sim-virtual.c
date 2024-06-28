// Lucas Demarco Cambraia Lemos - 2110013
// Natalia Passini Grossmann - 2110456

#include "sim-virtual.h"

int main(int argc, char *argv[]) {
  if (argc != 5) {
    printf("Número de parâmetros invalido.\n");
    exit(1);
  }
  char *algoritmo;
  char *algoritmo_arq = argv[1];
  char *nome_arquivo = argv[2];
  int tam_pagina = atoi(argv[3]);
  int tam_memoria = atoi(argv[4]);

  FILE *f = fopen(nome_arquivo, "r");
  if (f == NULL) {
    printf("Nome invalido para o arquivo.\n");
    exit(1);
  }
  if (strcmp("LRU", algoritmo_arq) == 0) {
    algoritmo = "LRU";
  } else if (strcmp("NRU", algoritmo_arq) == 0) {
    algoritmo = "NRU";
  } else {
    printf("Escolha entre os algoritmos LRU e NRU.\n");
    exit(1);
  }
  if (tam_pagina < 8 || tam_pagina > 16) {
    printf("O tamanho da pagina deve ser entre 8 e 16 KB.\n");
    exit(1);
  }
  if (tam_memoria < 1 || tam_memoria > 4) {
    printf("O tamanho de memoria deve ser entre 1 e 4 MB.\n");
    exit(1);
  }

  int pf, escritas;
  unsigned int addr;
  char rw;
  int tempo = 0;
  pf = 0;
  escritas = 0;
  int verifica;

  printf("Executando o simulador...\n");

  Tabela *tabela = inicializa_tabela((tam_memoria * 1024) / tam_pagina);

  while (fscanf(f, "%x %c ", &addr, &rw) == 2) {
    verifica = verifica_endereco(tabela, addr, tam_pagina, tempo, rw);
    if (verifica == 0) {
      pf++;
      add_pagina(tabela, addr, tam_pagina, algoritmo, tempo);
      if (rw == 'W') {
        escritas++;
      }
    }
    tempo++;
    for(int i = 0; i < tabela->capacidade; i++) {
      if(tabela->array_paginas[i] != NULL) {
        if((tempo - tabela->array_paginas[i]->ultimo_acesso) >= 5 && tabela->array_paginas[i]->usada == 1) {
          tabela->array_paginas[i]->usada = 0;
        }
      }
    }
  }

  printf("Arquivo de entrada: %s\n", nome_arquivo);
  printf("Tamanho da memoria fisica: %d MB\n", tam_memoria);
  printf("Tamanho da paginas: %d KB\n", tam_pagina);
  printf("Algoritmo de substituição: %s\n", algoritmo);
  printf("Numero de Faltas de Paginas: %d\n", pf);
  printf("Numero de Paginas Escritas: %d\n", escritas);
  return 0;
}

Tabela *inicializa_tabela(int capacidade) {
  Tabela *tabela = (Tabela *)malloc(sizeof(Tabela));
  if (tabela == NULL) {
    printf("Erro de malloc.\n");
    exit(1);
  }

  tabela->quantidade = 0;
  tabela->capacidade = capacidade;
  tabela->array_paginas = (Pagina **)malloc(sizeof(Pagina *) * capacidade);

  for (int i = 0; i < tabela->capacidade; i++) {
    tabela->array_paginas[i] = (Pagina *)malloc(sizeof(Pagina));
    tabela->array_paginas[i]->end = 0;
    tabela->array_paginas[i]->ultimo_acesso = -1;
    tabela->array_paginas[i]->modificada = 0;
    tabela->array_paginas[i]->usada = 0;
  }

  return tabela;
}

int verifica_endereco(Tabela *tabela, unsigned int addr, int tamanho, int tempo, char acao) {
  for (int i = 0; i < tabela->capacidade; i++) {
    if (tabela->array_paginas[i]->end == set_endereco(addr, tamanho)) {
      tabela->array_paginas[i]->usada = 1;
      tabela->array_paginas[i]->ultimo_acesso = tempo;
      if (acao == 'W') {
        tabela->array_paginas[i]->modificada = 1;
      } else {
        tabela->array_paginas[i]->modificada = 0;
      }
      return 1;
    }
  }
  return 0;
}

int set_endereco(int addr, int tamanho) {
  int auxiliar = tamanho;
  int contador = 0;
  while (auxiliar > 1) {
    auxiliar = auxiliar >> 1;
    contador++;
  }
  int endereco = addr >> (contador + 10);
  return endereco;
}

void add_pagina(Tabela *tabela, int addr, int tamanho, char *alg, int tempo) {
  if (tabela->quantidade < tabela->capacidade) {
    tabela->array_paginas[tabela->quantidade]->end =
        set_endereco(addr, tamanho);
    tabela->array_paginas[tabela->quantidade]->modificada = 0;
    tabela->array_paginas[tabela->quantidade]->usada = 0;
    tabela->quantidade++;
  } else {
    if (strcmp("NRU", alg) == 0) {
      alg_NRU(tabela, addr, tempo, tamanho);
    } else if (strcmp("LRU", alg) == 0) {
      alg_LRU(tabela, addr, tempo, tamanho);
    }
  }
}

void alg_LRU(Tabela *tabela, int addr, int tempo, int tamanho) {
  int tempo_aux = tabela->array_paginas[0]->ultimo_acesso;
  int posicao_pagina = 0;

  for (int i = 1; i < tabela->capacidade; i++) {
    if (tempo_aux > tabela->array_paginas[i]->ultimo_acesso) {
      posicao_pagina = i;
      tempo_aux = tabela->array_paginas[i]->ultimo_acesso;
    }
  }

  tabela->array_paginas[posicao_pagina]->modificada = 0;
  tabela->array_paginas[posicao_pagina]->usada = 1;
  tabela->array_paginas[posicao_pagina]->ultimo_acesso = tempo;
  tabela->array_paginas[posicao_pagina]->end = set_endereco(addr, tamanho);
}

void alg_NRU(Tabela *tabela, int addr, int tempo, int tamanho) {
  int modificada, usada, posicao_pagina;
  int posicao_pagina_caso_1 = -1;
  int posicao_pagina_caso_2 = -1;
  int posicao_pagina_caso_3 = -1;
  int posicao_pagina_caso_4 = -1;

  for (int i = 0; i < tabela->capacidade; i++) {
    if (tabela->array_paginas[i] != NULL) {
      usada = tabela->array_paginas[i]->usada;
      modificada = tabela->array_paginas[i]->modificada;
      
      if (modificada == 0 && usada == 0) {
        posicao_pagina_caso_1 = i;
        break;
      }

      if(modificada == 1 && usada == 0) {
        if(posicao_pagina_caso_2 < 0)
          posicao_pagina_caso_2 = i;
      }
      
      if(modificada == 0 && usada == 1) {
        if(posicao_pagina_caso_3 < 0)
          posicao_pagina_caso_3 = i;
      }
      
      if(modificada == 1 && usada == 1) {
        if(posicao_pagina_caso_4 < 0)
          posicao_pagina_caso_4 = i;
      }
    }
  }

  if(posicao_pagina_caso_1 >= 0) {
    posicao_pagina = posicao_pagina_caso_1;
  } else if(posicao_pagina_caso_2 >= 0) {
    posicao_pagina = posicao_pagina_caso_2;
  } else if(posicao_pagina_caso_3 >= 0) {
    posicao_pagina = posicao_pagina_caso_3;
  } else {
    posicao_pagina = posicao_pagina_caso_4;
  }
  
  tabela->array_paginas[posicao_pagina]->modificada = 0;
  tabela->array_paginas[posicao_pagina]->usada = 1;
  tabela->array_paginas[posicao_pagina]->ultimo_acesso = tempo;
  tabela->array_paginas[posicao_pagina]->end = set_endereco(addr, tamanho);
}
